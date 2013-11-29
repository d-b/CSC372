/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

// Short training symbol parameters
#define TRAINING_SHORT_POINTS    64
#define TRAINING_SHORT_AMPLITUDE 4.0

namespace modem
{
    ofdm::ofdm(parameters_t parameters,
               medium*      physical_medium,
               modulator*   subcarrier_modulator,
               stream*      packet_stream) :
        parameters(parameters),
        equalization_freqresponse(1, parameters.rate, parameters.points),
        training_short(1, parameters.rate),
        training_short_spectrum(1, parameters.rate, TRAINING_SHORT_POINTS)
    {
        ext.med  = physical_medium;
        ext.mod  = subcarrier_modulator;
        ext.strm = packet_stream;
        initialize_symbols();
    }

    void ofdm::initialize_symbols(void) {
        // Setup short training symbol
        spectrum& spec = training_short_spectrum;
        size_t    len  = training_short_spectrum.components;
        spec[0][2       + 4*0] = std::complex<double>(-1,  1);
        spec[0][2       + 4*1] = std::complex<double>( 1, -1);
        spec[0][2       + 4*2] = std::complex<double>( 1, -1);
        spec[0][len - 2 - 4*2] = std::complex<double>( 1,  1);
        spec[0][len - 2 - 4*1] = std::complex<double>( 1,  1);
        spec[0][len - 2 - 4*0] = std::complex<double>(-1, -1);
        // Synthesize the signal
        training_short = spec;
        training_short[0].erase(training_short[0].begin() + len/2, training_short[0].end());
        training_short *= TRAINING_SHORT_AMPLITUDE;
        // Preamble length must be even
        if(parameters.preamble_length % 2)
            parameters.preamble_length += 1;
    }
    
    void ofdm::insert_preamble(signal& sig) {
        sig = training_short * parameters.preamble_length + sig;
    }
    
    void ofdm::insert_cyclicprefix(signal& sig) {
        assert(parameters.cyclicprefix_length <= sig[0].size());
        size_t length = parameters.cyclicprefix_length;
        sig[0].insert(sig[0].begin(), sig[0].end() - length, sig[0].end());
    }

    bool ofdm::frame_test(const signal& sig) {
        // Symbol size
        size_t symsize = training_short[0].size();

        // See if our signal is long enough to contain the training sequence
        if(sig[0].size() < parameters.preamble_length * symsize)
            return false;

        // Perform correlation tests
        size_t len = parameters.preamble_length;
        for(int i = 0; i < len/2; i++) {
            // First and second
            signal symbol1(1, parameters.rate), symbol2(1, parameters.rate);
            symbol1[0].insert(symbol1[0].begin(), sig[0].begin() + (i + 0)       * symsize,
                                                  sig[0].end()   + (i + 1)       * symsize);
            symbol2[0].insert(symbol2[0].begin(), sig[0].begin() + (len - i - 1) * symsize,
                                                  sig[0].end()   + (len - i - 0) * symsize);
            
            // Compute correlation
            spectrum spec1(symbol1, TRAINING_SHORT_POINTS);
            spectrum spec2(symbol2, TRAINING_SHORT_POINTS);
            double correlation = abs(spec1.correlation(spec2));

            // Test 1: Check correlation between symbols
            if(correlation > parameters.threshold)
                // Test 2: Test correlation between symbol and expected training symbol
                correlation = abs(spec1.correlation(training_short_spectrum));
                if(correlation > parameters.threshold)
                    return true;
        }

        // No correlation found
        return false;
    }

    void ofdm::tick(double deltatime) {
        signal sig1(1, parameters.rate);
        signal sig2(1, parameters.rate);
        insert_preamble(sig2);
        ext.med->input(sig1);
        ext.med->output(sig2);
    }
}
