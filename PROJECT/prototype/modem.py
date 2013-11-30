#
# CSC372 - Project: Modem
#
# Daniel Bloemendal <d.bloemendal@gmail.com>
#

import os
import time
import numpy as np
import matplotlib.pyplot as plt
import multiprocessing as mp
from dsp import synthesize, upconvert, downconvert
from stream import Stream
from visualizations import Spectrum, Waveform

# Parameters
SAMPLE_RATE  = 48000
NYQUIST_RATE = 24000

# Main routine
def main():
  # Test sequence
  components = 1024
  stream = Stream(SAMPLE_RATE)
  spectrum = Spectrum(stream, components)
  waveform = Waveform(stream, components)
  y = synthesize([512] + [512]*128 + [0]*767 + [512]*128)
  y = upconvert(y, 10000, NYQUIST_RATE)
  y = downconvert(y, 10000, NYQUIST_RATE, 12000)
  stream.write(y)
  spectrum.tick()
  waveform.tick()
  plt.show()

# Entry point
if __name__ == '__main__':
  process = mp.Process(target=main)
  process.start()
  os._exit(0)
