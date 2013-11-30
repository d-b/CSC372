/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{
    // Data types
    typedef uint8_t byte;

    class medium {
    public:
        enum response { MEDIUM_Okay, MEDIUM_Error };
        virtual response input(signal& sig)        = 0;        
        virtual response output(const signal& sig) = 0;
    };

    class modulator {
    public:
        enum response { MODULATOR_Okay, MODULATOR_NeedMore, MODULATOR_DemodulationError };
        virtual response modulate(const std::vector<byte>& input, spectrum& output)   = 0;
        virtual response demodulate(std::vector<byte>& output, const spectrum& input) = 0;
    };    

    class stream {
    public:
        enum response { STREA_Okay, STREAM_Error };
        virtual response incomming(const std::vector<byte>& data) = 0;
        virtual response outgoing(std::vector<byte>& data)        = 0;
    };

    class ofdm {
    public:        
        typedef struct {
            // Base parameters
            uint16_t rate;
            uint16_t points;
            uint16_t symbols;
            double   carrier;
            double   bandwidth;
            double   threshold;

            // Preamble & cyclic prefix
            uint16_t preamble_length;
            uint16_t cyclicprefix_length;
        } 
        parameters_t;

    private:

        // External modules
        struct {
            medium*    med;
            modulator* mod;
            stream*    strm;
        } ext;

        // 
        // Common
        //

        // OFDM parameters
        parameters_t parameters;
        spectrum equalization_freqresponse;        

        // Training symbols
        signal   training_short;
        spectrum training_short_spectrum;        

        // Training symbol initialization
        void initialize_symbols(void);

        //
        // Sending
        //

        // Synchronization and interference mitigation mechanisms
        void insert_preamble(signal& sig);
        void insert_cyclicprefix(signal& sig);

        // Main loop
        void tick_sender(double deltatime);

        //
        // Receiving
        //

        signal frame;

        // Frame detection
        bool frame_test(const signal& sig);

        // Main loop
        void tick_receiver(double deltatime);

    public:
        ofdm(parameters_t parameters,
             medium*      physical_medium,
             modulator*   subcarrier_modulator,
             stream*      packet_stream);

        void tick(double deltatime);
    };
}
