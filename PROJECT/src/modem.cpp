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

    // Setup OFDM instance
    modem::ofdm ofdm(parameters);
    return 0;
}
