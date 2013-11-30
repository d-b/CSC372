/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{ 
    class modulator_qam : public modulator {
    public:
        modulator_qam(uint16_t points);

        response modulate(const std::vector<byte>& input, spectrum& output);
        response demodulate(std::vector<byte>& output, const spectrum& input);
    };
}