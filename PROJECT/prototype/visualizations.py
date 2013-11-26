#
# CSC372 - Project: Modem
#
# Daniel Bloemendal <d.bloemendal@gmail.com>
#

import matplotlib.pyplot as plt
import matplotlib.animation as anim
import numpy as np

class Spectrum:
    def __init__(self, stream, components):
        # Ensure an even number of components
        if(components % 2 != 0): components += 1

        # Initialize figure
        self.stream = stream
        self.figure = plt.figure()
        self.components = components

        # Setup x axis
        bins   = np.arange(-components // 2 + 1, components // 2)
        self.x = bins * 1.0 * (stream.rate/components)

        # Add plot
        self.subplot = self.figure.add_subplot(
            111, title='Spectrum', xlim=(self.x[0], self.x[-1]), ylim=(0, 2))
        self.y, = self.subplot.plot(self.x, np.zeros(components - 1))

    def tick(self):
        # Read samples and perform spectral analysis
        samples = self.stream.peek(self.components)
        spectrum = np.fft.fft(samples, self.components)/(self.components/2)

        # Update graph
        data = abs(np.hstack((spectrum[self.components//2 + 1: -1], spectrum[:self.components//2 + 1])))
        self.y.set_ydata(data)

class Waveform:
    def __init__(self, stream, samples):
        # Initialize figure
        self.stream  = stream
        self.figure  = plt.figure()
        self.samples = samples

        # Setup x axis
        self.x = np.arange(0, samples)

        # Add plots
        self.subplot = self.figure.add_subplot(
            111, title='Wave', xlim=(self.x[0], self.x[-1]), ylim=(-5, 5))
        self.y_real, = self.subplot.step(self.x, np.zeros(samples))
        self.y_imag, = self.subplot.step(self.x, np.zeros(samples))

    def tick(self):
        # Read samples
        data = self.stream.peek(self.samples)
        if(len(data) < self.samples):
            data += [0]*(self.samples - len(data))

        # Update graph
        self.y_real.set_ydata(np.real(data))
        self.y_imag.set_ydata(np.imag(data))
