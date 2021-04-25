#! /usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.signal import correlate, decimate, resample, windows, find_peaks
from samplerate import converters


samplerate0, sample0 = wavfile.read('/home/Pedro/Documents/AUDIO/Snare.wav')
samplerate1, sample1 = wavfile.read('/home/Pedro/Documents/AUDIO/Kick.wav')


RootFolder = '/home/Pedro/Documents/AUDIO/Samples/'

# rsample = converters.resample(sample0, 0.5)
# rsample = converters.resample(rsample, 2)

# # rsample = resample(sample0, 2)
# # rsample = resample(rsample, len(sample0)* 2)

# wavfile.write(RootFolder + 'SnareTest_.wav', samplerate0, np.array(rsample))
# exit(0)
# wavfile.write(RootFolder + 'SnareFast.wav', samplerate0, np.array([ x for i,x in enumerate(sample0) if not (i % 2) ]))

# snareFast = []
# for i,x in enumerate(sample0):
#     snareFast += [x]
#     if i != 0:
#         snareFast += [(sample0[i - 1] + x) / 2]

# wavfile.write(RootFolder + 'SnareSlow.wav', samplerate0, np.array(snareFast))


# size = len(sample)
size = 1024

# exit(0)

fig, canvas = plt.subplots(2,2)

h = windows.hamming(size)
windowed = sample0[:size] * h
corr = correlate(windowed, windowed) / 64

corr = corr[-len(corr):]

canvas[0][0].plot(sample0[:size])
canvas[1][0].plot(corr)
peaks, _ = find_peaks(corr, height=0.1)
canvas[1][0].plot(peaks, corr[peaks], 'x')

peaks = peaks - peaks[0]
print(peaks)

sine = np.array([ np.sin(x* 2.0*np.pi*441.0/44100.0) for x in range(44100 * 3) ][:size])
# wavfile.write(RootFolder + "Sine1.wav", 44100, np.array(sine))
canvas[0][1].plot(sine)

var = sine.var()
sine = sine - sine.mean()
corrSine = correlate(sine * h, sine * h)[-size:]
r = corrSine / (var*(np.arange(size, 0, -1)))
peaksSine, _ = find_peaks(corrSine, height=0.2)
canvas[1][1].plot(corrSine)
canvas[1][1].plot(peaksSine, corrSine[peaksSine], 'x')
print(peaksSine)
print(peaksSine - peaksSine[0])


plt.show()
