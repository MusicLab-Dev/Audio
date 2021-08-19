/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator
 */

#pragma once

#include "Generator.hpp"

namespace Audio::DSP
{
    template<unsigned InstanceCount>
    struct Oscillator;
}

template<unsigned InstanceCount>
struct Audio::DSP::Oscillator
{
public:
    using PhaseIndexCache = std::array<std::array<float, InstanceCount>, KeyCount>;

    void resetKey(const Key key) noexcept { _phaseIndex[key].fill(0.0f); }

    template<unsigned Index>
    void resetKey(const Key key) noexcept { _phaseIndex[key] = 0.0f; }

    void reset(void) noexcept
    {
        for (auto &key : _phaseIndex)
            key.fill(0.0f);
    }


    /** @brief Generate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void generate(const Generator::Waveform waveform, Type *output, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::Generate<Accumulate>(waveform, output, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }

    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void generate(const Generator::Waveform waveform, Type *output, const Type *input, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::Generate<Accumulate>(waveform, output, input, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void modulate(const Generator::Waveform waveform, Type *output, const Type *modulation, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::Modulate<Accumulate>(waveform, output, modulation, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void modulate(const Generator::Waveform waveform, Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::Modulate<Accumulate>(waveform, output, input, modulation, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void semitoneShift(const Generator::Waveform waveform, Type *output, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::SemitoneShift<Accumulate>(waveform, output, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void semitoneShift(const Generator::Waveform waveform, Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::SemitoneShift<Accumulate>(waveform, output, input, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void modulateSemitoneShift(const Generator::Waveform waveform, Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::ModulateSemitoneShift<Accumulate>(waveform, output, modulation, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }
    template<bool Accumulate, unsigned Index = 0u, typename Type>
    void modulateSemitoneShift(const Generator::Waveform waveform, Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const DB gain) noexcept
    {
        const auto nextPhase = Generator::ModulateSemitoneShift<Accumulate>(waveform, output, input, modulation, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index], gain);
        incrementPhaseIndex<Index>(key, nextPhase);
    }

private:
    PhaseIndexCache _phaseIndex;

    template<unsigned Index>
    void incrementPhaseIndex(const Key key, const float nextPhase) noexcept
    {
        const float cycle = 2.0f * static_cast<float>(M_PI);

        if (const float dt = nextPhase / cycle; dt >= 1.0f) {
            _phaseIndex[key][Index] = nextPhase - static_cast<std::size_t>(dt) * cycle;
        } else
            _phaseIndex[key][Index] = nextPhase;
    }
};
