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
    return np.fft.ifft(spectrum)*2.0

def upconvert(signal, frequency):
    n = len(signal)
    converted = np.array([signal[i]*np.exp(2j*np.pi*frequency*(i/n)) for i, x in enumerate(signal)])
    return np.real(converted)

def downconvert(signal, frequency, bandwidth, nyquist):
    n = len(signal)
    converted = np.array([signal[i]*np.exp(2j*np.pi*frequency*(i/n)) for i, x in enumerate(signal)])
    return lowpass(converted, bandwidth/2, nyquist)
