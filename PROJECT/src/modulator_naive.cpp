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
        std::cout << "Spectrum (" << input.components << "):" << std::endl;
        for(int i = 0; i < input.components; i++) {
            double mag = abs(input[0][i]);
            if(mag > 0.25)
                std::cout << "X[" << i << "]: " << mag << std::endl;
        } return MODULATOR_DemodulationError;
    }
}
