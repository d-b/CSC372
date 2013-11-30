/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem
{
    modulator::response modulator_naive::modulate(const std::vector<byte>& input, spectrum& output) {
        output[0][1   ] = 4.0;
        output[0][1023] = 4.0;
        return MODULATOR_Okay;
    }
    
    modulator::response modulator_naive::demodulate(std::vector<byte>& output, const spectrum& input) {
        return MODULATOR_DemodulationError;
    }
}
