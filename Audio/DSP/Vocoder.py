#! /usr/bin/python3

import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile
from samplerate import resample
import matplotlib.pyplot as plt

ANALYSIS_SIZE = 256
ANALYSIS_HOP_SIZE = ANALYSIS_SIZE // 4

nbplot = 10
fig, canvas = plt.subplots(nbplot,1)
plt.subplots_adjust(top=0.95, bottom=0.05, left=0.05, right=0.95, wspace=0.05)

# samplerate, sample = wavfile.read('/home/Pedro/Documents/AUDIO/SnareCrop.wav')
samplerate, sample = wavfile.read('/home/Pedro/Documents/AUDIO/Sine440.wav')
sample = list(sample)
sample = sample[:-30]


# wavfile.write("/home/Pedro/Documents/AUDIO/Sine440.wav", 48000, np.array([np.sin(2.0*np.pi*440.0*x/48000.0) for x in range(48000 // 3)]))
# exit(1)


N_INIT_INPUT = len(sample)
# sample = [0.0] * (3*ANALYSIS_HOP_SIZE) + sample
# sample = sample + [0.0] * (3*ANALYSIS_HOP_SIZE)
if len(sample) % ANALYSIS_HOP_SIZE != 0:
    sample += ([0.0] * int(ANALYSIS_HOP_SIZE - (len(sample) % ANALYSIS_HOP_SIZE)))



N_INPUT = len(sample)
N_BLOCK = N_INPUT // ANALYSIS_HOP_SIZE - 3

print(N_INPUT / ANALYSIS_HOP_SIZE)
print(N_INIT_INPUT, N_INPUT, N_BLOCK)

window = np.hanning(ANALYSIS_SIZE)

analysisFrames = []

for i in range(N_BLOCK):
    print(i)
    analysisFrames.append(sample[i*ANALYSIS_HOP_SIZE:i*ANALYSIS_HOP_SIZE+ANALYSIS_SIZE])
    print(len(analysisFrames[-1]), i*ANALYSIS_HOP_SIZE, i*ANALYSIS_HOP_SIZE+ANALYSIS_SIZE)


# print(len(analysisFrames))
for i in range(nbplot):
    # canvas[i].set_ylim([-0.1,0.1])
    # canvas[i].plot(analysisFrames[i+61])
    canvas[i].plot(sample)

plt.show()