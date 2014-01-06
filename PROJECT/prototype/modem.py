#
# CSC372 - Project: Modem
#
# Daniel Bloemendal <d.bloemendal@gmail.com>
#

import os
import time
import numpy as np
import scipy.io.wavfile
import scipy as sp
import matplotlib.pyplot as plt
import multiprocessing as mp
from dsp import synthesize, upconvert, downconvert
from stream import Stream
from visualizations import Spectrum, Waveform

# Parameters
SAMPLE_RATE  = 48000
NYQUIST_RATE = 24000
COMPONENETS  = 64

# Main routine
def main():
  # Start interactive mode
  plt.ion()

  # Load sample
  wavfile  = sp.io.wavfile.read('sample.wav')
  wavdata  = np.float64(wavfile[1])
  wavdata /= np.float64(np.iinfo(wavfile[1].dtype).max)
  wavdata *= 10.0
  stream   = Stream(wavfile[0])
  stream.write(wavdata)

  # Search for training symbol
  training_spectrum = np.array([complex() for x in range(64)])
  training_spectrum[2      + 4*0] = complex(1, 0)
  training_spectrum[2      + 4*1] = complex(1, 0)
  training_spectrum[2      + 4*2] = complex(1, 0)
  training_spectrum[64 - 2 - 4*2] = complex(1, 0)
  training_spectrum[64 - 2 - 4*1] = complex(1, 0)
  training_spectrum[64 - 2 - 4*0] = complex(1, 0)
  training_signal = np.fft.ifft(training_spectrum)*40.0
  training_spectrum /= np.linalg.norm(training_spectrum)
  training_signal   /= np.linalg.norm(training_signal)
  for i in range(len(stream.buffer) - 64):
    window_signal  = stream.buffer[i: i + 64]
    window_signal /= np.linalg.norm(window_signal)
    #window_spectrum = np.fft.fft(window_signal)
    #window_spectrum /= np.linalg.norm(window_spectrum)
    corr = abs(np.correlate(training_signal, window_signal))
    print(corr)
    return
    if(corr > 0.9):
      print('Signal @ %s' % str(i/SAMPLE_RATE))
      stream.pop(i)
      break

  # Display signal
  spectrum = Spectrum(stream, COMPONENETS)
  waveform = Waveform(stream, COMPONENETS)
  spectrum.tick()
  waveform.tick()
  plt.show()
  while(True):
    plt.pause(0.10)
    stream.pop(1)
    spectrum.tick()
    waveform.tick()

# Entry point
if __name__ == '__main__':
  process = mp.Process(target=main)
  process.start()
  os._exit(0)
