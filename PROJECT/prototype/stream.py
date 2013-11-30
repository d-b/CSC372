#
# CSC372 - Project: Modem
#
# Daniel Bloemendal <d.bloemendal@gmail.com>
#

from collections import deque

class Stream:
    def __init__(self, rate):
        self.rate = rate
        self.buffer = []
    def peek(self, length):
        return self.buffer[:length]
    def pop(self, length):
        self.buffer = self.buffer[length:len(self.buffer)]
    def read(self, length):
        data = self.peek(length)
        self.pop(length)
        return data
    def write(self, data):
        self.buffer += list(data)
