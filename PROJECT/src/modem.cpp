/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    Spectrum spec;
    OFDM ofdm(spec, 1024, 64);
    return 0;
}
