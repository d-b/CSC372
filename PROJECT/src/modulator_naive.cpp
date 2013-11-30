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
        static int counter = 0;
        output[0][counter++ % 50] = 10.0;
        return MODULATOR_Okay;
    }
    
    modulator::response modulator_naive::demodulate(std::vector<byte>& output, const spectrum& input) {
        std::cout << "Spectrum (" << input.components << "):" << std::endl;
        spectrum normalized(input);
        normalized /= normalized.abs();
        for(int i = 0; i < normalized.components; i++) {
            double mag = abs(normalized[0][i]);
            if(mag > 0.5)
                std::cout << "X[" << i << "]: " << mag << std::endl;
        } return MODULATOR_DemodulationError;
    }
}
