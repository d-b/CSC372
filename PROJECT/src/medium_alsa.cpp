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

    medium_alsa::medium_alsa(const char* device, uint16_t rate, size_t buffer_size)
        : rate(rate), buffer_size(buffer_size), device(device)
    {
        buffer.resize(buffer_size);
        initialize_alsa();
    }

    void medium_alsa::initialize_alsa() {
        snd_pcm_hw_params_t *hw_params;
        std::stringstream error; int res;
        unsigned int samplerate = rate;
    
        if ((res = snd_pcm_open(&handle_input, device.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
            error << "cannot open audio device " << snd_strerror(res) << res;
            throwx(alsa_exception(error.str()));
        }
           
        if ((res = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
            error << "cannot allocate hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
                 
        if ((res = snd_pcm_hw_params_any(handle_input, hw_params)) < 0) {
            error << "cannot initialize hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_access(handle_input, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
            error << "cannot initialize hardware parameters structure (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_format(handle_input, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
            error << "cannot set sample format (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_rate_near(handle_input, hw_params, &samplerate, 0)) < 0) {
            error << "cannot set sample format (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params_set_channels(handle_input, hw_params, 1)) < 0) {
            error << "cannot set channel count (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        }
    
        if ((res = snd_pcm_hw_params(handle_input, hw_params)) < 0) {
            error << "cannot set channel count (" << snd_strerror(res) << ")";
            throwx(alsa_exception(error.str()));
        } snd_pcm_hw_params_free (hw_params);
    }


    medium::response medium_alsa::output(const signal& sig) {

    }
    
    medium::response medium_alsa::input(signal& sig) {

    }
}
