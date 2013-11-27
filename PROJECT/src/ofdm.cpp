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
    {}
    
    void ofdm::insert_preamble(signal& sig, uint16_t length) {
    }
    
    void ofdm::insert_cyclicprefix(signal& sig, uint16_t length) {
    }

    bool ofdm::frame_test(signal& sig, size_t* offset) {
        return false;
    }
}
