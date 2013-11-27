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

            // Preamble
            uint16_t preamble_length;
            uint16_t preamble_frequency;

            // Cyclic prefix
            uint16_t cyclicprefix_length;
        } parameters_t;

    private:
        // OFDM parameters
        parameters_t parameters;

        // Equalization parameters
        spectrum equalization_freqresponse;

        // Training & pilot symbols
        signal training_short;
        signal training_long;

        // Synchronization and interference mitigation mechanisms
        void insert_preamble(signal& sig, uint16_t length);
        void insert_cyclicprefix(signal& sig, uint16_t length);

        // Frame detection
        bool frame_test(signal& sig, size_t* offset = NULL);

    public:
        ofdm(parameters_t parameters);
    };
}
