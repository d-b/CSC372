/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem
{
    ofdm::ofdm(parameters_t parameters)
        : parameters(parameters),
          equalization_freqresponse(1, parameters.rate, parameters.points),
          training_short(1, parameters.rate), training_long(1, parameters.rate)
    {initialize_symbols();}

    void ofdm::initialize_symbols(void) {
        // Setup short training symbol
        spectrum spec(1, parameters.rate, 64);
        spec[0][ 2 + 4*0] = std::complex<double>(-1,  1);
        spec[0][ 2 + 4*1] = std::complex<double>( 1, -1);
        spec[0][ 2 + 4*2] = std::complex<double>( 1, -1);
        spec[0][62 - 4*2] = std::complex<double>( 1,  1);
        spec[0][62 - 4*1] = std::complex<double>( 1,  1);
        spec[0][62 - 4*0] = std::complex<double>(-1, -1);
        // Synthesize the signal
        training_short = spec;
        training_short[0].erase(training_short[0].begin() + 32, training_short[0].end());
        training_short *= 4.0;
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

    bool ofdm::frame_test(signal& sig) {
        // Symbol size
        size_t symsize = training_short[0].size();

        // See if our signal is long enough to contain the training symbol
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
            double correlation
                = abs(spectrum(symbol1, 64).correlation(spectrum(symbol2, 64)));

            // Check correlation
            if(correlation > parameters.threshold)
                return true;
        }

        // No correlation found
        return false;
    }
}
