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
        // Common
        parameters(parameters),
        equalization_freqresponse(1, parameters.rate, parameters.points),
        training_short(1, parameters.rate),
        training_short_spectrum(1, parameters.rate, TRAINING_SHORT_POINTS),
        receiver_frame(1, parameters.rate),
        sender_frame_count(0)
    {
        // Initialize internals
        ext.med  = physical_medium;
        ext.mod  = subcarrier_modulator;
        ext.strm = packet_stream;
        initialize_symbols();

        // Start receiver in initial state
        receiver_goto(RSTATE_WaitingForSignal);
    }

    //
    // Sender
    //

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

    
    void ofdm::sender_tick(double deltatime) {
        // If medium is not currently in handling output bail out
        if(!(ext.med->mode() | medium::DUPLEX_Output)) return;

        // Fetch data from the stream
        std::vector<byte> data;
        stream::response sres = ext.strm->outgoing(data);
        if(sres != stream::STREAM_Okay || data.empty()) return;

        // Send data to the modulator
        spectrum spec(1, parameters.rate, parameters.points);
        if(ext.mod->modulate(data, spec) == modulator::MODULATOR_Okay) {
            // Synthesize the signal and insert the cyclic prefix
            signal frame(spec);
            insert_cyclicprefix(frame);
            // Convert the signal up to passband
            frame.upconvert(parameters.carrier);
            // See if we need to add a preamble
            if(sender_frame_count % parameters.symbols == 0) {
                insert_preamble(frame);
                sender_frame_count = 0;
            }
            // Pass the frame on to the medium
            ext.med->output(frame);
            sender_frame_count += 1;
        }
    }

    //
    // Receiver
    //

    int ofdm::frame_test(const signal& sig) {
        // Symbol size
        size_t symsize = training_short[0].size();

        // See if our signal is long enough to contain the training sequence
        if(sig[0].size() < parameters.preamble_length * symsize)
            return FRAME_NeedMore;

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
            if(correlation > parameters.threshold) {
                // Test 2: Test correlation between symbol and expected training symbol
                correlation = abs(spec1.correlation(training_short_spectrum));
                if(correlation > parameters.threshold)
                    return FRAME_Okay;
            }
        }

        // No correlation found
        return FRAME_NotFound;
    }

    void ofdm::receiver_process(signal& frame) {
        // Compute the spectrum and perform subcarrier demodulation
        std::vector<byte> payload;
        spectrum spec(frame, parameters.points);
        modulator::response res = ext.mod->demodulate(payload, spec); switch(res) {
            case modulator::MODULATOR_Okay:
                // Pass the payload onto the stream handler
                ext.strm->incomming(payload); break;
            case modulator::MODULATOR_NeedMore:
                break;
            case modulator::MODULATOR_DemodulationError:
                receiver_frame_errors += 1;
                break;
        }
    }

    void ofdm::receiver_training(void) {
        // TODO: add coarse and fine frequency adjustment techniques
    }

    void ofdm::receiver_goto(rstate state) {
        switch(state) {
            case RSTATE_WaitingForSignal:
                receiver_state = RSTATE_WaitingForSignal;
                receiver_frame.clear();
                receiver_frame_errors = 0;
                break;

            case RSTATE_WaitingForFrame:
                receiver_state = RSTATE_WaitingForFrame;
                receiver_frame_count = 0;
                break;
        }
    }

    void ofdm::receiver_tick(double deltatime) {
        // If medium is not currently in handling input reset receiver state and bail out
        if(!(ext.med->mode() | medium::DUPLEX_Input)) {
            receiver_goto(RSTATE_WaitingForSignal); return;
        }

        // Read data from medium
        signal sig(1, parameters.rate);
        ext.med->input(sig);
        if(sig[0].empty()) return;
        receiver_frame += sig;

        switch(receiver_state) {
            // Signal acquisition phase
            case RSTATE_WaitingForSignal: {
                // Test frame for signal
                int result = frame_test(receiver_frame);
                // If a frame was found
                if(result == FRAME_Okay) {
                    // Perform training operation
                    receiver_training();
                    // Remove preamble from frame
                    size_t samples = parameters.preamble_length * training_short[0].size();
                    samples = std::min(receiver_frame[0].size(), samples);
                    receiver_frame[0].erase(receiver_frame[0].begin(), receiver_frame[0].begin() + samples);
                    // Start waiting for OFDM frames
                    receiver_goto(RSTATE_WaitingForFrame);
                } else if(result == FRAME_NotFound)
                    receiver_frame.clear();
            }; break;

            // Frame processing phase
            case RSTATE_WaitingForFrame: {
                // See if we have a complete frame
                size_t frame_size = parameters.points + parameters.cyclicprefix_length;
                if(receiver_frame[0].size() >= frame_size) {
                    // Convert the signal from passband down to baseband
                    signal frame(1, parameters.rate);
                    frame[0].insert(frame[0].begin(), receiver_frame[0].begin(), receiver_frame[0].begin() + frame_size);
                    receiver_frame[0].erase(receiver_frame[0].begin(), receiver_frame[0].begin() + frame_size);
                    frame.downconvert(parameters.carrier, parameters.bandwidth);
                    // Remove the cyclic prefix
                    frame[0].erase(frame[0].begin(), frame[0].begin() + parameters.cyclicprefix_length);
                    // Process the frame
                    receiver_process(frame);
                    receiver_frame_count += 1;
                    // See if we need to switch back to the signal acquisition state
                    if(receiver_frame_count >= parameters.symbols)
                        receiver_goto(RSTATE_WaitingForSignal);
                }
            }; break;
        }
    }

    void ofdm::tick(double deltatime) {
        receiver_tick(deltatime); // Receiver update
        sender_tick  (deltatime); // Sender update
    }
}
