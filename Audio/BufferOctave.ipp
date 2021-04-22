/**
 * @ Author: Pierre Veysseyre
 * @ Description: BufferOctave.ipp
 */

#include <Audio/DSP/Resampler.hpp>

template<typename Type>
inline void Audio::GenerateOctave(const BufferView input, BufferOctave &octave) noexcept
{
    DSP::Resampler<Type> resampler;

    const std::size_t inputSize = input.size<Type>();
    std::size_t nextSize = DSP::Resampler<Type>::GetResampleSemitoneBufferSize(inputSize, false);
    // std::size_t nextSize = DSP::Resampler<Type>::GetResampleOctaveBufferSize(inputSize, -1);
    std::size_t lastSize = inputSize;
    std::size_t lastIdx = OctaveRootKey;

    constexpr auto GetMinMax = [](const BufferView buffer) {
        const auto size = buffer.size<float>();
        const float *data = buffer.data<float>();
        float min = 1000.f;
        float max = -1000.f;

        for (auto i = 0ul; i < size; ++i) {
            min = std::min(min, data[i]);
            max = std::max(max, data[i]);
        }
        std::cout << "min: " << min << " max: " << max << std::endl;
    };

    std::cout << "REF sample:" << std::endl;
    GetMinMax(octave[OctaveRootKey]);
    // Lowest notes
    for (auto i = 0u; i < OctaveRootKey; ++i) {
        const std::size_t nextIdx = lastIdx - 1;
        octave[nextIdx].resize(nextSize * sizeof(Type), octave[lastIdx].sampleRate(), octave[lastIdx].channelArrangement(), octave[lastIdx].format());
        octave[nextIdx].clear();
        resampler.template resampleSemitone<false, 8u>(octave[lastIdx].data<Type>(), octave[nextIdx].data<Type>(), lastSize, octave[OctaveRootKey].sampleRate(), false);
        // resampler.template resampleOctave<false, 8u>(octave[lastIdx].data<Type>(), octave[nextIdx].data<Type>(), lastSize, octave[OctaveRootKey].sampleRate(), 1);
        GetMinMax(octave[nextIdx]);
        lastSize = nextSize;
        lastIdx = nextIdx;
        nextSize = DSP::Resampler<Type>::GetResampleSemitoneBufferSize(nextSize, false);
        // nextSize = DSP::Resampler<Type>::GetResampleOctaveBufferSize(nextSize, -1);
    }
    nextSize = DSP::Resampler<Type>::GetResampleSemitoneBufferSize(inputSize, true);
    // nextSize = DSP::Resampler<Type>::GetResampleOctaveBufferSize(inputSize, 1);
    lastSize = inputSize;
    lastIdx = OctaveRootKey;
    // Highest notes
    for (auto i = 0u; i < KeysPerOctave - OctaveRootKey - 1; ++i) {
        const std::size_t nextIdx = lastIdx + 1;
        octave[nextIdx].resize(nextSize * sizeof(Type), octave[lastIdx].sampleRate(), octave[lastIdx].channelArrangement(), octave[lastIdx].format());
        octave[nextIdx].clear();
        resampler.template resampleSemitone<false, 8u>(octave[lastIdx].data<Type>(), octave[nextIdx].data<Type>(), lastSize, octave[OctaveRootKey].sampleRate(), true);
        // resampler.template resampleOctave<false, 8u>(octave[lastIdx].data<Type>(), octave[nextIdx].data<Type>(), lastSize, octave[OctaveRootKey].sampleRate(), 1);
        GetMinMax(octave[nextIdx]);
        lastSize = nextSize;
        lastIdx = nextIdx;
        nextSize = DSP::Resampler<Type>::GetResampleSemitoneBufferSize(nextSize, true);
        // nextSize = DSP::Resampler<Type>::GetResampleOctaveBufferSize(nextSize, 1);
    }
}
