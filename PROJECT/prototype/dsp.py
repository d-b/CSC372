#
# CSC372 - Project: Modem
#
# Daniel Bloemendal <d.bloemendal@gmail.com>
#

import scipy.signal
import numpy as np
import scipy as sp

def synthesize(spectrum):
    return np.fft.ifft(spectrum)

def lowpass(signal, cutoff, nyquist):
    # Spectrum parameters
    n     = (len(signal) - 1)//2
    width = nyquist/n
    # Compute spectrum
    spectrum = np.fft.fft(signal)
    # Kill bins above cutoff
    start = int(np.ceil(cutoff/width))
    for i in range(start, n + 1):
        spectrum[ i] = 0
        spectrum[-i] = 0
    # Kill Nyquist bin as necessary
    if((len(signal) - 1) % 2):
        spectrum[n + 1] = 0
    return np.fft.ifft(spectrum)

def upconvert(signal, frequency, nyquist, points):
    # Choose nearest frequency bin
    n         = points//2
    width     = nyquist/n
    frequency = width * np.ceil(frequency/width)
    # Perform frequency shift and return real value result
    period = (nyquist/frequency)*2.0
    converted = np.array([signal[i]*np.exp(2j*np.pi*(i/period)) for i, x in enumerate(signal)])
    return np.real(converted)*2.0

def downconvert(signal, frequency, nyquist, bandwidth, points):
    # Choose nearest frequency bin
    n         = points//2
    width     = nyquist/n
    frequency = width * np.ceil(frequency/width)
    # Perform frequency shift and apply low-pass filter
    period = (nyquist/frequency)*2.0
    converted = np.array([signal[i]*np.exp(-2j*np.pi*(i/period)) for i, x in enumerate(signal)])
    return lowpass(converted, bandwidth/2, nyquist)
