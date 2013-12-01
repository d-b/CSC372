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
        static std::vector<byte> data;
        if(data.size() <= 1024)
            data.insert(data.begin(), input.begin(), input.end());
        if(!data.empty()) {
            output[0][data[0]] = 10.0;
            data.erase(data.begin());
        } return MODULATOR_Okay;
    }
    
    modulator::response modulator_naive::demodulate(std::vector<byte>& output, const spectrum& input) {
        spectrum normalized(input);
        normalized /= normalized.abs();
        for(int i = 0; i < normalized.components; i++) {
            double mag = abs(normalized[0][i]);
            if(mag > 0.5)
                std::cout << (char) i << std::flush;
        } return MODULATOR_DemodulationError;
    }
}
