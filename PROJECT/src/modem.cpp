/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

void usage(const char* application) {
    std::cout << "CSC372 - Bench 101" << std::endl
              << "Daniel Bloemendal <d.bloemendal@gmail.com>" << std::endl
              << "David Lu <david-lu@hotmail.com>" << std::endl
              << "Usage: " << application
              << " [-i input mode] [-o output mode]" << std::endl;
}

int main(int argc, char* argv[]) {
    // OFDM parameters
    modem::ofdm::parameters_t parameters = {
        48000, /* rate                */
        1024,  /* points              */
        128,   /* symbols             */
        5000,  /* carrier             */
        9000,  /* bandwidth           */
        0.75,  /* threshold           */
    
        10,    /* preamble_length     */
        128,   /* cyclicprefix_length */
    };

    // Timeout parameters
    timeval timeout = {
        1, /* tv_sec  */
        0, /* tv_usec */
    };


    // I/O mode
    int io_mode = modem::medium::DUPLEX_None;

    // Process arguments
    int option;
    while ((option = getopt (argc, argv, "io")) != -1)
        switch (option) {
            case 'i':
            io_mode |= modem::medium::DUPLEX_Input;
            break;

            case 'o':
            io_mode |= modem::medium::DUPLEX_Output;
            break;

            default:
                usage(argv[0]);
                return -1;
        }

    // Check for full duplex
    if(io_mode == modem::medium::DUPLEX_None) {
        usage(argv[0]); return -1;
    }
    else if(io_mode == modem::medium::DUPLEX_Full) {
        std::cout << "Full duplex mode is not supported!" << std::endl; return -1;
    }


    // Setup OFDM instance
    modem::stream_hello strm;
    modem::modulator_naive mod;
    modem::medium_alsa med("default", "default", parameters.rate, 4096, 64, static_cast<modem::medium::duplex>(io_mode));
    modem::ofdm ofdm(parameters, &med, &mod, &strm);
    
    // Main loop
    for(;;) {
        med.wait(&timeout);
        med.tick(0);
        ofdm.tick(0);
    }

    return 0;
}
