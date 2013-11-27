/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

int main(int argc, char* argv[]) {
    modem::spectrum spec(1, 48000, 1024);
    modem::signal sig(spec);
    modem::soundfile::save("test.wav", sig);
    return 0;
}
