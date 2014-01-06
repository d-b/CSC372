/*
 * CSC372 - Project: Modem
 *
 * Daniel Bloemendal <d.bloemendal@gmail.com>
 * David Lu <david-lu@hotmail.com>
 */

#include "modem.hpp"

namespace modem {
    namespace soundfile {
        int save(std::string filename, const signal& sig) {
            // Open sound file
            SF_INFO sfinfo    = {0};
            sfinfo.samplerate = sig.rate;
            sfinfo.channels   = sig.channels;
            sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
            SNDFILE* sndfile  = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
            if(!sndfile) return -1;

            // Write signal to file
            std::vector<double> frames;
            frames.resize(sig.samples()*sig.channels);
            for(int i = 0; i < sig.channels; i++)
                for(int j = 0; j < sig[i].size(); j++)
                    frames[sig.channels*j + i] = std::real(sig[i][j]);
            sf_writef_double(sndfile, frames.data(), sig.samples());

            // Cleanup and return
            sf_close(sndfile);
            return 0;
        }

        std::shared_ptr<signal> load(std::string filename) {
            // Open sound file
            SF_INFO sfinfo = {0};
            SNDFILE* sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
            if(!sndfile) return nullptr;

            // Construct a new signal and return it
            std::vector<double> frames;
            frames.resize(sfinfo.frames * sfinfo.channels);
            sf_readf_double(sndfile, frames.data(), sfinfo.frames);
            std::shared_ptr<signal> sig(new signal(sfinfo.channels, sfinfo.samplerate));
            for(int i = 0; i < sfinfo.channels; i++) {
                (*sig)[i].resize(sfinfo.frames);
                for(int j = 0; j < sfinfo.frames; j++)
                    (*sig)[i][j] = std::complex<double>(frames[sfinfo.channels*j + i], 0);
            }
            return sig;
        }
    }
}