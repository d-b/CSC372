/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

int main(int argc, char* argv[]) {
    // OFDM parameters
    modem::ofdm::parameters_t parameters = {
        48000, /* rate                */
        1024,  /* points              */
        16,    /* symbols             */
        5000,  /* carrier             */
        9000,  /* bandwidth           */
        0.75,  /* threshold           */
    
        10,     /* preamble_length     */
        128,    /* cyclicprefix_length */
    };

    // Timeout parameters
    timeval timeout = {
        1, /* tv_sec  */
        0, /* tv_usec */
    };

    // Setup OFDM instance
    modem::stream_hello strm;
    modem::modulator_naive mod;
    modem::medium_alsa med("default", "default", parameters.rate, 4096, 64, modem::medium::DUPLEX_Output);
    modem::ofdm ofdm(parameters, &med, &mod, &strm);
    
    // Main loop
    for(;;) {
        med.wait(&timeout);
        med.tick(0);
        ofdm.tick(0);
    }

    return 0;
}
