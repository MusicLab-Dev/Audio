/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator
 */

#pragma once

#include "Generator.hpp"
#include "Phase.hpp"

namespace Audio::DSP
{
    template<unsigned InstanceCount>
    struct Oscillator;
}

template<unsigned InstanceCount>
struct Audio::DSP::Oscillator
{
public:
    using PhaseIndexCache = std::array<std::array<Phase<float>, InstanceCount>, KeyCount>;

    void resetKey(const Key key) noexcept { _phaseIndex[key].fill(0.0f); }

    template<unsigned Index>
    void resetKey(const Key key) noexcept { _phaseIndex[key][Index].reset(); }

    void reset(void) noexcept
    {
        for (auto &key : _phaseIndex)
            key.fill(0.0f);
    }

    template<unsigned Index>
    void setPhase(const Key key, const float phase) noexcept { _phaseIndex[key][Index].setPhase(phase); }

    template<unsigned Index>
    [[nodiscard]] float phase(const Key key) const noexcept { return _phaseIndex[key][Index].phase(); }


    /** @brief Generate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void generate(const Generator::Waveform waveform, Type *output, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::Generate<Accumulate>(waveform, output, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::Generate<Accumulate>(waveform, output, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }

    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void generate(const Generator::Waveform waveform, Type *output, const Type *input, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::Generate<Accumulate>(waveform, output, input, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::Generate<Accumulate>(waveform, output, input, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void modulate(const Generator::Waveform waveform, Type *output, const Type *modulation, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::Modulate<Accumulate>(waveform, output, modulation, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::Modulate<Accumulate>(waveform, output, modulation, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void modulate(const Generator::Waveform waveform, Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::Modulate<Accumulate>(waveform, output, input, modulation, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::Modulate<Accumulate>(waveform, output, input, modulation, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void semitoneShift(const Generator::Waveform waveform, Type *output, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::SemitoneShift<Accumulate>(waveform, output, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::SemitoneShift<Accumulate>(waveform, output, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void semitoneShift(const Generator::Waveform waveform, Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::SemitoneShift<Accumulate>(waveform, output, input, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::SemitoneShift<Accumulate>(waveform, output, input, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void modulateSemitoneShift(const Generator::Waveform waveform, Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::ModulateSemitoneShift<Accumulate>(waveform, output, modulation, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::ModulateSemitoneShift<Accumulate>(waveform, output, modulation, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }
    template<bool Accumulate, unsigned Index = 0u, bool IncrementPhase = true, typename Type>
    void modulateSemitoneShift(const Generator::Waveform waveform, Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const Key key, const float frequencyNorm, const std::uint32_t indexOffset, const DB gain) noexcept
    {
        if constexpr (IncrementPhase) {
            _phaseIndex[key][Index].setPhase(Generator::ModulateSemitoneShift<Accumulate>(waveform, output, input, modulation, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain));
        } else {
            Generator::ModulateSemitoneShift<Accumulate>(waveform, output, input, modulation, semitone, outputSize, frequencyNorm, _phaseIndex[key][Index].phase(), indexOffset, gain);
        }
    }

private:
    PhaseIndexCache _phaseIndex;

    template<unsigned Index>
    void incrementPhaseIndex(const Key key, const float nextPhase) noexcept
    {
        float phase = nextPhase;
        while (phase >= Pi2F)
            phase -= Pi2F;
        _phaseIndex[key][Index].setPhase(phase);
    }
};
