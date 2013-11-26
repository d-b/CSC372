#
# CSC372 - Project: Modem
#
# Daniel Bloemendal <d.bloemendal@gmail.com>
#

import numpy as np

def upconvert(signal, frequency):
    n = len(signal)
    converted = np.array([np.exp(2j*np.pi*frequency*(i/n)) for i, x in enumerate(signal)])
    return np.real(converted)

def downconvert(signal, frequency):
    return signal
