/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem
{ 
    medium_alsa::alsa_exception::alsa_exception(const std::string& message)
        : message(message)
    {};

    std::string medium_alsa::alsa_exception::compute(const std::string& file, int line) {
        std::stringstream ss;
        ss << "ALSA exception in " << file << " @ line " << line << ": " << message;
        return ss.str().c_str();
    }

    medium_alsa::medium_alsa(const char* device_input, const char* device_output, uint16_t rate, size_t buffer_size, size_t period_size, duplex io_mode)
        : rate(rate), buffer_size(buffer_size), period_size(period_size), io_mode(io_mode)
    {
        buffer.resize(buffer_size);
        if(io_mode & DUPLEX_Input ) initialize_device(&handle_input,  device_input,  SND_PCM_STREAM_CAPTURE );
        if(io_mode & DUPLEX_Output) initialize_device(&handle_output, device_output, SND_PCM_STREAM_PLAYBACK);
        initialize_polling();
    }

    void medium_alsa::initialize_device(snd_pcm_t** handle, const char* device, snd_pcm_stream_t stream) {
        snd_pcm_uframes_t buffer_size = this->buffer_size;
        snd_pcm_uframes_t period_size = this->period_size;
        snd_pcm_hw_params_t *hw_params;
        snd_pcm_sw_params_t *sw_params;
        std::stringstream error;
        int res, dir;

        //
        // Hardware parameters
        //
    
        if((res = snd_pcm_open(handle, device, stream, SND_PCM_NONBLOCK)) < 0) {
            error << "cannot open audio device " << snd_strerror(res) << res;
            throwx(alsa_exception(error.str()));
        }
           
        if((res = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
            error << "cannot allocate hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
                 
        if((res = snd_pcm_hw_params_any(*handle, hw_params)) < 0) {
           error << "cannot initialize hardware parameters structure (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }
    
        if((res = snd_pcm_hw_params_set_access(*handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
           error << "cannot initialize hardware parameters structure (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }
    
        if((res = snd_pcm_hw_params_set_format(*handle, hw_params, SND_PCM_FORMAT_FLOAT)) < 0) {
           error << "cannot set sample format (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }
    
        if((res = snd_pcm_hw_params_set_rate(*handle, hw_params, rate, 0)) < 0) {
           error << "cannot set sample rate (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }
    
        if((res = snd_pcm_hw_params_set_channels(*handle, hw_params, 1)) < 0) {
           error << "cannot set channel count (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }

        if((res = snd_pcm_hw_params_set_buffer_size_near(*handle, hw_params, &buffer_size)) < 0) {
           error << "cannot set buffer size (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }

        if((res = snd_pcm_hw_params_set_period_size_near(*handle, hw_params, &period_size, &dir)) < 0) {
           error << "cannot set period size (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        } 
    
        if((res = snd_pcm_hw_params(*handle, hw_params)) < 0) {
            error << "cannot set hardware parameters (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
        
        snd_pcm_hw_params_free(hw_params);

        //
        // Software parameters
        //

        if((res = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
           error << "cannot allocate software parameters structure (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }

        if((res = snd_pcm_sw_params_current(*handle, sw_params)) < 0) {
           error << "cannot allocate software parameters structure (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }

        if((res = snd_pcm_sw_params_set_avail_min(*handle, sw_params, period_size)) < 0) {
           error << "cannot set minimum available count (" << snd_strerror(res) << ")";
           throwx(alsa_exception(error.str()));
        }

        if((res = snd_pcm_sw_params(*handle, sw_params)) < 0) {
            error << "cannot set software parameters (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        //
        // Setup interface
        //

        if((res = snd_pcm_prepare(*handle)) < 0) {
            error << "cannot prepare audio interface for use (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        if((res = snd_pcm_start(*handle)) < 0) {
            error << "cannot stop interface (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    }


    void medium_alsa::initialize_polling() {
        // Preparations
        size_t size;
        std::stringstream error; int res;

        // Setup input device descriptors
        if(mode() & DUPLEX_Input) {
            ufds_input.resize(snd_pcm_poll_descriptors_count(handle_input));
            if((res = snd_pcm_poll_descriptors(handle_input, ufds_input.data(), ufds_input.size())) < 0) {
                error << "cannot acquire polling descriptors (" << snd_strerror(res) << ")";
                throwx(alsa_exception(error.str()));
            }
        }
        // Setup output device descriptors
        else if(mode() & DUPLEX_Output) {
            ufds_output.resize(snd_pcm_poll_descriptors_count(handle_output));
            if((res = snd_pcm_poll_descriptors(handle_output, ufds_output.data(), ufds_output.size())) < 0) {
                error << "cannot acquire polling descriptors (" << snd_strerror(res) << ")";
                throwx(alsa_exception(error.str()));
            }
        }
    }

    void medium_alsa::initialize_fdsets(fd_set* read_fds, fd_set* write_fds, size_t* max) {
        // Add FDs for input device
        for(auto fd = ufds_input.begin(); fd != ufds_input.end(); ++fd) {
            if(fd->events & POLLIN ) FD_SET(fd->fd, read_fds);
            if(fd->events & POLLOUT) FD_SET(fd->fd, write_fds);
            if(fd->events & (POLLIN | POLLOUT))
                if(fd->fd > *max) *max = fd->fd;
        }

        // Conditionally add FDs for output device
        if(!buffer_output.empty())
            for(auto fd = ufds_output.begin(); fd != ufds_output.end(); ++fd) {
                if(fd->events & POLLIN ) FD_SET(fd->fd, read_fds);
                if(fd->events & POLLOUT) FD_SET(fd->fd, write_fds);
                if(fd->events & (POLLIN | POLLOUT))
                    if(fd->fd > *max) *max = fd->fd;
            }
    }

    int medium_alsa::wait(timeval* timeout){
        // Initialize FD sets
        size_t maxfd = -1;
        fd_set read_fds ; FD_ZERO(&read_fds );
        fd_set write_fds; FD_ZERO(&write_fds);
        initialize_fdsets(&read_fds, &write_fds, &maxfd);
        if(maxfd < 0) return 0;

        // Perform select
        return select(maxfd + 1, &read_fds, &write_fds, NULL, timeout);
    }

    medium::duplex medium_alsa::mode(void) {
        return io_mode;
    }

    size_t medium_alsa::free() {
        return buffer_size - buffer_output.size();
    }

    medium::response medium_alsa::input(signal& sig) {
        sig[0].insert(sig[0].end(), buffer_input.begin(), buffer_input.end());
        buffer_input.clear();
        return MEDIUM_Okay;
    }

    medium::response medium_alsa::output(const signal& sig) {
        if(buffer_output.size() + sig[0].size() > buffer_size) return MEDIUM_Again;
        buffer_output.insert(buffer_output.end(), sig[0].begin(), sig[0].end());
        return MEDIUM_Okay;
    }

    void medium_alsa::frames_read(void) {
        // Frames read
        snd_pcm_sframes_t frames = 0;

        // Resize buffer to fit data
        buffer.resize(buffer_size);

        // Read frames from device
        if((frames = snd_pcm_readi(handle_input, buffer.data(), buffer.size())) > 0)
            buffer_input.insert(buffer_input.end(), buffer.begin(), buffer.begin() + frames);
        else if(frames != -EAGAIN) {
            std::stringstream error; int res;
            if((res = snd_pcm_recover(handle_input, frames, 1)) < 0) {
                error << "cannot recover from input error (" << snd_strerror(res) << ")";
                throwx(alsa_exception(error.str()));
            }
        }        
    }
    
    void medium_alsa::frames_write(void) {
        // Frames written
        snd_pcm_sframes_t frames = 0;

        for(;;) {
            // Fetch chunk size
            size_t size = std::min((size_t) buffer_size, buffer_output.size());
            if(size <= 0) break;

            // Add chunk to buffer            
            buffer.clear(); std::for_each(buffer_output.begin(), buffer_output.begin() + size,
                [this](const std::complex<double>& val){
                    this->buffer.push_back(real(val));
                });

            // Write frames to device
            if((frames = snd_pcm_writei(handle_output, buffer.data(), size)) > 0)
                buffer_output.erase(buffer_output.begin(), buffer_output.begin() + frames);
            else if(frames != -EAGAIN) {
                std::stringstream error; int res;
                if((res = snd_pcm_recover(handle_output, frames, 1)) < 0) {
                    error << "cannot recover from output error (" << snd_strerror(res) << ")";
                    throwx(alsa_exception(error.str()));
                }
            }
            else break;
        }
    }

    void medium_alsa::tick(double deltatime) {
        if(mode() & DUPLEX_Input ) frames_read();
        if(mode() & DUPLEX_Output) frames_write(); 
    }
}
