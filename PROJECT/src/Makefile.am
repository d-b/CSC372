bin_PROGRAMS = modem
modem_SOURCES = modem.cpp ofdm.cpp soundfile.cpp signal.cpp medium_alsa.cpp modulator_naive.cpp modulator_qam.cpp stream_text.cpp
modem_CFLAGS = @FFTW3_CFLAGS@ @SNDFILE_CFLAGS@ @ALSA_CFLAGS@
modem_CPPFLAGS = @FFTW3_CFLAGS@ @SNDFILE_CFLAGS@ @ALSA_CFLAGS@
modem_LDADD = @FFTW3_LIBS@ @SNDFILE_LIBS@ @ALSA_LIBS@
