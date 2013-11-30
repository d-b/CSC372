/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{ 
    class modulator_naive : public modulator {
    public:
        response modulate(const std::vector<byte>& input, spectrum& output);
        response demodulate(std::vector<byte>& output, const spectrum& input);
    };
}