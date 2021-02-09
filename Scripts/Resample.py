#! /usr/bin/python3

from sys import argv
import numpy as np

SIZE = 34567

if len(argv) > 1:
    SIZE = int(argv[1])


def dec(size, semitone):
    pass

def inter(size, semitone):
    pass

SEMITONE_L_0 = 185
SEMITONE_M_0 = 196

SEMITONE_L_1 = 7450
SEMITONE_M_1 = 7893


sizes = []
def resampSemitone(size, semitone):
    size = int(size)
    L = SEMITONE_L_0
    M = SEMITONE_M_0
    print("resampleSemitone", size, "->", semitone)
    if semitone < 0:
        if len(sizes) == 0:
            sizes.append(np.ceil((size + (size - 1) * (L - 1)) / M))
        else:
            sizes.append(np.ceil((sizes[-1] + (sizes[-1] - 1) * (L - 1)) / M))
    else:
        if len(sizes) == 0:
            sizes.append(np.ceil((size + (size - 1) * (M - 1)) / L))
        else:
            sizes.append(np.ceil((sizes[-1] + (sizes[-1] - 1) * (M - 1)) / L))

    print("\t", sizes)


def resampSize(size, newSize):
    size = int(size)
    newSize = int(np.ceil(newSize))
    print("resampleSize", size, "->", newSize)
    gcd = np.gcd(size, newSize)

    L = newSize / gcd
    M = size / gcd

    print("L:", L, "M:", M)

    iSize = size + (size - 1) * (L - 1)
    dSize = np.ceil(iSize / M)
    print(gcd, "\t",iSize, dSize)


def process(up = True):
    for i in range(1, 3):
        print()
        if up:
            print("up", i, "semitone")
            alpha = pow(2, -i / 12)
        else:
            print("down", i, "semitone")
            alpha = pow(2, i / 12)
        newSize = SIZE * alpha
        # resampSize(SIZE, newSize)
        print("EXCEPTED:", SIZE, newSize)
        if up:
            resampSemitone(SIZE, -i)
        else:
            resampSemitone(SIZE, i)
    print()

process(True)
sizes = []
process(False)

print(sizes)

# resampSize(44100, 48000)
# process(False)