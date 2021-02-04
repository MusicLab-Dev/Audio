#! /usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter


N = 1024
sampleRate = 48000
cutOff = 2048
gain = 6
q = 0.707
qAsBandWidth = False

omega = 0
tsin = 0
tcos = 0
alpha = 0

def init():
    print(omega, tsin, tcos, alpha)
    omega = 2.0 * np.pi * cutOff / sampleRate
    tsin = np.sin(omega)
    tcos = np.cos(omega)
    print(omega, tsin, tcos, alpha)
    # if qAsBandWidth:
    #     alpha = (tsin * np.sinh(np.log(2.0) / 2.0 * q * omega / tsin))
    # else:
    #     alpha = (tsin / (2.0 * q))

a = [0.0] * 3
b = [0.0] * 3

def lp():
    init()
    print(omega, tsin, tcos, alpha)
    b[0] = (1.0 - tcos) / 2.0
    b[1] = 1.0 - tcos
    b[2] = (1.0 - tcos) / 2.0
    a[0] = 1.0 + alpha
    a[1] = -2.0 * tcos
    a[2] = 1.0 - alpha

    b[0] = b[0] / a[0]
    b[1] = b[1] / a[0]
    b[2] = b[2] / a[0]
    a[1] = a[1] / a[0]
    a[2] = a[2] / a[0]
    a[0] = 1

def hp():
    init()
    b[0] = (1.0 + tcos) / 2.0
    b[1] = -(1.0 + tcos)
    b[2] = (1.0 + tcos) / 2.0
    a[0] = 1.0 + alpha
    a[1] = -2.0 * tcos
    a[2] = 1.0 - alpha

    b[0] = b[0] / a[0]
    b[1] = b[1] / a[0]
    b[2] = b[2] / a[0]
    a[1] = a[1] / a[0]
    a[2] = a[2] / a[0]
    a[0] = 1

def bp():
    init()
    b[0] = alpha
    b[1] = 0
    b[2] = -alpha
    a[0] = 1.0 + alpha
    a[1] = -2.0 * tcos
    a[2] = 1.0 - alpha

    b[0] = b[0] / a[0]
    b[1] = b[1] / a[0]
    b[2] = b[2] / a[0]
    a[1] = a[1] / a[0]
    a[2] = a[2] / a[0]
    a[0] = 1


ax = plt.subplot(111)
ax.set_xscale("log")
ax.set_ylim([-80, 20])
ax.vlines(cutOff, -100, 20)
ax.get_xaxis().set_major_formatter(ScalarFormatter())

fftA = None
fftB = None

step = int(sampleRate / N)
xs = [ (x + 1) * step for x in range(N//2)]

lp()
fftA = np.fft.fft(a, N)
fftB = np.fft.fft(b, N)
f = 20.0 * np.log10(np.abs(fftB * fftB) / (fftA * fftA))
ax.plot(xs, f[:N//2])

# cutOff = 8000
# hp()
# sumA = fftA
# sumB = fftB
# fftA = np.fft.fft(a, N)
# fftB = np.fft.fft(b, N)
# f = 20.0 * np.log10(np.abs(fftB) / fftA)
# ax.plot(xs, f[:N//2])

# s = 20.0 * np.log10(np.abs(fftB * sumB) / (fftA * sumA))
# ax.plot(xs, s[:N//2])



plt.show()
