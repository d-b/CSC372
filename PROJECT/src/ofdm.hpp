/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{
    class ofdm {
    public:        
        typedef struct {
            // Base parameters
            uint16_t rate;
            uint16_t points;
            uint16_t carrier;
            uint16_t bandwidth;
            uint16_t symbols;
            double   threshold;

            // Preamble & cyclic prefix
            uint16_t preamble_length;
            uint16_t cyclicprefix_length;
        } parameters_t;

    private:
        // OFDM parameters
        parameters_t parameters;

        // Equalization parameters
        spectrum equalization_freqresponse;

        // Training symbols
        signal training_short;
        signal training_long;

        // Training symbol initialization
        void initialize_symbols(void);

        // Synchronization and interference mitigation mechanisms
        void insert_preamble(signal& sig);
        void insert_cyclicprefix(signal& sig);

        // Frame detection
        bool frame_test(signal& sig);

    public:
        ofdm(parameters_t parameters);
    };
}
