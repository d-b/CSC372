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
        enum duplex   { DUPLEX_Input = 1, DUPLEX_Output = 2, DUPLEX_Full = (DUPLEX_Input | DUPLEX_Output) };
        enum response { MEDIUM_Okay, MEDIUM_Error };
        virtual duplex   mode(void)                = 0;
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
        enum response { STREAM_Okay, STREAM_Error };
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
        // Sender
        //

        size_t sender_frame_count;

        // Synchronization and interference mitigation mechanisms
        void insert_preamble(signal& sig);
        void insert_cyclicprefix(signal& sig);

        // Main loop
        void sender_tick(double deltatime);

        //
        // Receiver
        //

        enum rstate {
            RSTATE_WaitingForSignal,
            RSTATE_WaitingForFrame,
        };

        rstate receiver_state;
        signal receiver_frame;
        size_t receiver_frame_count;
        size_t receiver_frame_errors;

        // Frame detection
        bool frame_test(const signal& sig);

        // Main loop
        void receiver_process(signal& frame);
        void receiver_training(void);
        void receiver_goto(rstate state);
        void receiver_tick(double deltatime);

    public:
        ofdm(parameters_t parameters,
             medium*      physical_medium,
             modulator*   subcarrier_modulator,
             stream*      packet_stream);

        void tick(double deltatime);
    };
}
