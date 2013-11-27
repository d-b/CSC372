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
            SF_INFO sfinfo;
            sfinfo.samplerate = sig.rate;
            sfinfo.channels   = sig.channels;
            sfinfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
            SNDFILE* sndfile  = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
            if(!sndfile) return -1;

            // Write signal to file
            for(int i = 0; i < sig.samples(); i++) {
                double sample[sig.channels];
                for(int j = 0; j < sig.channels; j++)
                    sample[j] = std::real(sig[j][i]);
                sf_writef_double(sndfile, sample, 1);
            }

            // Cleanup and return
            sf_close(sndfile);
            return  0;
        }
    }
}