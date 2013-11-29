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
        5000,  /* carrier             */
        0,     /* bandwidth           */
        64,    /* symbols             */
        0.0,   /* threshold           */
    
        10,    /* preamble_length     */
        16     /* cyclicprefix_length */
    };

    // Timeout parameters
    timeval timeout = {
        1, /* tv_sec  */
        0, /* tv_usec */
    };

    // Setup OFDM instance
    modem::ofdm ofdm(parameters, NULL, NULL, NULL);
    modem::medium_alsa med("default", "default", 48000, 1024, 128);
    
    // Main loop
    for(;;) {
        med.wait(&timeout);
        med.tick(0);
        ofdm.tick(0);
    }

    return 0;
}
