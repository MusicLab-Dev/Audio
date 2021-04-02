/**
 * @ Author: Pierre Veysseyre
 * @ Description: BufferOctave.ipp
 */

#include <Audio/DSP/Resampler.hpp>

template<typename Type>
inline void Audio::GenerateOctave(const BufferView input, BufferOctave &octave) noexcept
{
    const std::size_t inputSize = input.size<Type>();
    std::size_t nextSize = DSP::GetResamplingSizeOneSemitone(inputSize, false);
    std::size_t lastSize = inputSize;
    std::size_t lastIdx = OctaveRootKey;

    // Lowest notes
    for (auto i = 0u; i < OctaveRootKey; ++i) {
        const std::size_t nextIdx = lastIdx - 1;
        octave[nextIdx].resize(nextSize * sizeof(Type), octave[lastIdx].sampleRate(), octave[lastIdx].channelArrangement(), octave[lastIdx].format());
        DSP::Resampler::ResampleSemitone<false>(octave[lastIdx].data<Type>(), octave[nextIdx].data<Type>(), lastSize, octave[OctaveRootKey].sampleRate(), false);
        lastSize = nextSize;
        lastIdx = nextIdx;
        nextSize = DSP::GetResamplingSizeOneSemitone(nextSize, false);
    }
    nextSize = DSP::GetResamplingSizeOneSemitone(inputSize, true);
    lastIdx = OctaveRootKey;
    // Highest notes
    for (auto i = 0u; i < KeysPerOctave - OctaveRootKey - 1; ++i) {
        const std::size_t nextIdx = lastIdx + 1;
        octave[nextIdx].resize(nextSize * sizeof(Type), octave[lastIdx].sampleRate(), octave[lastIdx].channelArrangement(), octave[lastIdx].format());
        DSP::Resampler::ResampleSemitone<false>(octave[lastIdx].data<Type>(), octave[nextIdx].data<Type>(), nextSize, octave[OctaveRootKey].sampleRate(), true);
        lastSize = nextSize;
        lastIdx = nextIdx;
        nextSize = DSP::GetResamplingSizeOneSemitone(nextSize, true);
    }
}
