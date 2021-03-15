#! /usr/bin/python3

import matplotlib.pyplot as plt
from scipy.signal import firwin, filtfilt, lfilter_zi, lfilter, butter, correlate, decimate, resample, windows, find_peaks, iirfilter
import numpy as np
from scipy.io import wavfile
from playsound import playsound

sinePath = '/home/Pedro/Documents/AUDIO/Sine440.wav'
snarePath = '/home/Pedro/Documents/AUDIO/SnareCrop.wav'
kickPath = '/home/Pedro/Documents/AUDIO/Kick.wav'
chordPath = '/home/Pedro/Documents/AUDIO/chord.wav'
sineSr, sine = wavfile.read(sinePath)
snareSr, snare = wavfile.read(snarePath)
kickSr, kick = wavfile.read(kickPath)
chordSr, chord = wavfile.read(chordPath)


L = 196
M = 185

def inter(data, ratio):
    ret = [0.0] * (len(data) + (len(data) - 1) * (ratio - 1))
    s = len(data)
    for i in range(s):
        ret[i * ratio] = data[i]
    return ret

def deci(data, ratio):
    ret = [0.0] * (len(data) // ratio)
    for i in range(len(ret)):
        ret[i] = data[i * ratio]
    return ret

def filter(data, sr, cutOff):
    q = 0.707
    omega = 2.0 * np.pi * cutOff / sr
    tsin = np.sin(omega)
    tcos = np.cos(omega)
    alpha = (tsin / (2.0 * q))
    a = [0.0] * 3
    b = [0.0] * 3
    b[0] = (1.0 - tcos) / 2.0
    b[1] = 1.0 - tcos
    b[2] = (1.0 - tcos) / 2.0
    a[0] = 1.0 + alpha
    a[1] = -2.0 * tcos
    a[2] = 1.0 - alpha
    b[0] /= a[0]
    b[1] /= a[0]
    b[2] /= a[0]
    a[1] /= a[0]
    a[2] /= a[0]

    regs = [0.0] * 2

    print (a, b)
    ret = [0.0] * len(data)
    for i in range(len(data)):
        out = data[i] * a[0] + regs[0];
        regs[0] = data[i] * a[1] + regs[1] - b[1] * out;
        regs[1] = data[i] * a[2] - b[2] * out;
        ret[i] = out
    return ret

def filter2(data, sr, cutOff):
    n = 2048
    a = firwin(n, cutoff=cutOff/sr)
    return lfilter(a, 1.0, data) * L

data = kick
# playsound(snarePath)
dataSize = len(data)
fData = filter2(data, 44100, 100)

iData = inter(data, L)
iRate = dataSize * L
fiData = filter2(iData, 44100, (44100.0) / M)
# fiData = filter(fiData, 44100 // 3)
# fiData = filter(fiData, 44100 // 3)
# fiData = filter(fiData, 44100 // 3)
# fdData = filter2(iData, 44100, 44100 // 2 / M)
dData = deci(fiData, M)

left = [
    data,
    iData,
    fiData,
    # fdData,
    dData
]
right = [ x for x in left ]

fig, canvas = plt.subplots(len(left), 2)
for i, data in enumerate(left):
    canvas[i][0].plot(range(len(data)), data)

for i, data, in enumerate(right):
    canvas[i][1].set_xscale('log')
    # canvas[i][1].set_yscale('log')
    # canvas[i][1].set_ylim([])
    canvas[i][1].plot(range(len(data)), np.abs(data) / 1)

wavfile.write('wav_chord.wav', chordSr, np.array(dData, dtype=float))
playsound('wav_chord.wav')


plt.show()