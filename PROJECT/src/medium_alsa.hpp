/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#pragma once

namespace modem
{ 
    class medium_alsa : public medium {
        // Device parameters
        const uint16_t    rate;
        const uint16_t    buffer_size;
        const std::string device;

        // ALSA device initialization
        std::vector<uint8_t> buffer;
        snd_pcm_t* handle_input;
        snd_pcm_t* handle_output;
        void initialize_alsa();

    public:
        // ALSA exception
        class alsa_exception : public exception {
            std::string message;
        public:
            alsa_exception(const std::string& message);
            virtual std::string compute(const std::string& file, int line);
        };

        // Construct an ALSA medium with the specified sample rate and buffer size
        medium_alsa(const char* device, uint16_t rate, size_t buffer_size);

        // Input/utput
        response output(const signal& sig);
        response input(signal& sig);
    };
}
