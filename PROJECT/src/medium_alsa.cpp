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

    medium_alsa::medium_alsa(const char* device_input, const char* device_output, uint16_t rate, size_t buffer_size, size_t period_size)
        : rate(rate), buffer_size(buffer_size), period_size(period_size)
    {
        initialize_device(&handle_input, device_input, SND_PCM_STREAM_CAPTURE);
        initialize_device(&handle_output, device_input, SND_PCM_STREAM_PLAYBACK);
    }

    void medium_alsa::initialize_device(snd_pcm_t** handle, const char* device, snd_pcm_stream_t stream) {
        unsigned int rate = this->rate;
        snd_pcm_uframes_t buffer_size = this->buffer_size;
        snd_pcm_uframes_t period_size = this->period_size;
        snd_pcm_hw_params_t *hw_params;
        snd_pcm_sw_params_t *sw_params;
        std::stringstream error;
        int res, dir;

        //
        // Hardware parameters
        //
    
        if ((res = snd_pcm_open(handle, device, stream, 0)) < 0) {
            error << "cannot open audio device " << snd_strerror(res) << res;
            throwx(alsa_exception(error.str()));
        }
           
        if ((res = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
            error << "cannot allocate hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
                 
        if ((res = snd_pcm_hw_params_any(*handle, hw_params)) < 0) {
            error << "cannot initialize hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_access(*handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
            error << "cannot initialize hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_format(*handle, hw_params, SND_PCM_FORMAT_FLOAT64)) < 0) {
            error << "cannot set sample format (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_rate_near(*handle, hw_params, &rate, 0)) < 0) {
            error << "cannot set sample format (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_channels(*handle, hw_params, 1)) < 0) {
            error << "cannot set channel count (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        if ((res = snd_pcm_hw_params_set_buffer_size_near(*handle, hw_params, &buffer_size)) < 0) {
            error << "cannot set buffer size (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        if ((res = snd_pcm_hw_params_set_period_size_near(*handle, hw_params, &period_size, &dir)) < 0) {
            error << "cannot set period size (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }  
    
        if ((res = snd_pcm_hw_params(*handle, hw_params)) < 0) {
            error << "cannot set hardware parameters (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
        
        snd_pcm_hw_params_free(hw_params);

        //
        // Software parameters
        //

        if ((res = snd_pcm_sw_params_malloc(&sw_params)) < 0) {
            error << "cannot allocate software parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        if ((res = snd_pcm_sw_params_current(*handle, sw_params)) < 0) {
            error << "cannot allocate software parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        if ((res = snd_pcm_sw_params_set_avail_min(*handle, sw_params, period_size)) < 0) {
            error << "cannot set minimum available count (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        if ((res = snd_pcm_sw_params(*handle, sw_params)) < 0) {
            error << "cannot set software parameters (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }

        // Prepare interface for use
        if ((res = snd_pcm_prepare(*handle)) < 0) {
            error << "cannot prepare audio interface for use (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    }


    medium::response medium_alsa::output(const signal& sig) {

    }
    
    medium::response medium_alsa::input(signal& sig) {

    }
}
