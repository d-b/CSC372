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
        const uint16_t rate;
        const uint16_t buffer_size;
        const uint16_t period_size;
        const duplex   io_mode;

        // Buffers
        std::vector<float>              buffer;
        std::deque<std::complex<float>> buffer_input;
        std::deque<std::complex<float>> buffer_output;

        // ALSA device initialization
        snd_pcm_t* handle_input;
        snd_pcm_t* handle_output;
        void initialize_device(snd_pcm_t** handle, const char* device, snd_pcm_stream_t stream);

        // ALSA device polling
        std::vector<pollfd> ufds_input;
        std::vector<pollfd> ufds_output;
        void initialize_polling();
        void initialize_fdsets(fd_set* read_fds, fd_set* write_fds, size_t* max);

        // Read and write to device
        void frames_read(void);
        void frames_write(void);

    public:
        // ALSA exception
        class alsa_exception : public exception {
            std::string message;
        public:
            alsa_exception(const std::string& message);
            virtual ~alsa_exception() throw() {};
            virtual std::string compute(const std::string& file, int line);
        };

        // Construct an ALSA medium with the specified sample rate and buffer size
        medium_alsa(const char* device_input, const char* device_output, uint16_t rate, size_t buffer_size, size_t period_size, duplex io_mode);

        // Progress
        void tick(double deltatime);
        int  wait(timeval* timeout);

        // Input/output
        duplex   mode(void);
        size_t   free(void);
        response input(signal& sig);
        response output(const signal& sig);
    };
}
