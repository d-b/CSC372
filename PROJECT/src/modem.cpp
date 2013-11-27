/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

int main(int argc, char* argv[]) {
    // Construct spectrum
    modem::spectrum spec(1, 48000, 1024);
    for(int i = 0; i <= 128; i++)
        spec[0][i] = 1.0;
    for(int i = 1024 - 128; i < 1024; i++)
        spec[0][i] = 1.0;
    // Synthesize signal
    modem::signal sig(spec);
    // Write up-converted signal
    sig.upconvert(14000);
    modem::soundfile::save("signal-up.wav", sig);
    // Write down-converted signal
    sig.downconvert(14000, 12000);
    modem::soundfile::save("signal-down.wav", sig);    
    return 0;
}
