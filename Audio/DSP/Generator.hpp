/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator
 */

#pragma once

#include <Audio/Base.hpp>
#include <Audio/Math.hpp>
#include <Audio/MathConstants.hpp>

#define GENERATOR_REGISTER_WAVEFORM(Name) \
    template<bool Accumulate, typename Type> \
    float Generate##Name(Type *output, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::GenerateImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float Generate##Name(Type *output, const Type *input, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::GenerateImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, input, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float Modulate##Name(Type *output, const Type *modulation, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, modulation, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float Modulate##Name(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float SemitoneShift##Name(Type *output, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::SemitoneShiftImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float SemitoneShift##Name(Type *output, const Type *input, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::SemitoneShiftImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, input, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float ModulateSemitoneShift##Name(Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateSemitoneShiftImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, modulation, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); } \
    template<bool Accumulate, typename Type> \
    float ModulateSemitoneShift##Name(Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const float phaseOffset = 0.0f, const std::uint32_t indexOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateSemitoneShiftImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, input, modulation, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }


namespace Audio::DSP::Generator
{
    enum Waveform : std::uint8_t {
        Sine = 0,
        Cosine,
        Square,
        Triangle,
        Saw,
        Noise,
        PulseThird,
        PulseQuarter,
        SquareAnalog,
        SawAnalog,
        PulseThirdAnalog,
        PulseQuarterAnalog
    };


    struct NoiseGenerator
    {
        Utils::RandomGenerator generator;
        std::array<float, KeyCount> lastRand;

        template<typename Type>
        void rand(const Key key) noexcept
        {

        }
    };

    struct AntiAlliasing
    {
        [[nodiscard]] static inline float GetCorrection(const float phaseNormOne, const float freqNormOne) noexcept
        {
            float phase = 0.0f;

            if (phaseNormOne < freqNormOne) {
                phase = phaseNormOne / freqNormOne;
                return phase + phase - phase * phase - 1.0f;
            }
            else if (phaseNormOne > 1.0f - freqNormOne) {
                phase = (phaseNormOne - 1.0f) / freqNormOne;
                return phase * phase + phase + phase + 1.0f;
            }
            return 0.0f;
        }
    };



    namespace Internal
    {
        [[nodiscard]] float IncrementPhase(const float phase, const float phaseInc) noexcept
        {
            float ph = phase + phaseInc;
            while (ph >= Pi2F)
                ph -= Pi2F;
            return ph;
        }


        /** @brief Generate implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float GenerateImpl(Type *output, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float GenerateImpl(Type *output, const Type *input, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;

        /** @brief Modulate implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float ModulateImpl(Type *output, const Type *modulation, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float ModulateImpl(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;

        /** @brief Modulate with semitone delta implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float SemitoneShiftImpl(Type *output, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float SemitoneShiftImpl(Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;

        /** @brief Modulate with semitone delta implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float ModulateSemitoneShiftImpl(Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline float ModulateSemitoneShiftImpl(Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept;


        /** @brief Waveform sample generation */
        template<typename Type>
        [[nodiscard]] Type GenerateSineSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(indexNorm) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateCosineSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::cos(indexNorm) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateSquareSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return indexNorm > PiF ? static_cast<Type>(gain) : static_cast<Type>(-gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateTriangleSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((1.0f - std::acos(std::sin(indexNorm)) * static_cast<float>(M_2_PI)) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateSawSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((2.0f * indexNorm / Pi2F - 1.0f) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateNoiseSample(const float indexNorm, const float, const std::uint32_t index, const DB gain = 1.0f) noexcept;
        template<typename Type>
        [[nodiscard]] Type GeneratePulseThirdSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return indexNorm > Pi2F / 3.0f ? static_cast<Type>(gain) : static_cast<Type>(-gain); }
        template<typename Type>
        [[nodiscard]] Type GeneratePulseQuarterSample(const float indexNorm, const float, const std::uint32_t, const DB gain = 1.0f) noexcept
            { return indexNorm > PiF / 2.0f ? static_cast<Type>(gain) : static_cast<Type>(-gain); }

        template<typename Type>
        [[nodiscard]] Type GenerateSquareAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const DB gain = 1.0f) noexcept
        {
            const float phaseCorrectionNorm = indexNorm / Pi2F;
            const float freqCorrectionNorm = freqNorm / Pi2F;
            float out = GenerateSquareSample<float>(indexNorm, freqNorm, index);

            out -= AntiAlliasing::GetCorrection(phaseCorrectionNorm, freqCorrectionNorm);
            out += AntiAlliasing::GetCorrection(std::fmod(phaseCorrectionNorm + 0.5f, 1.0f), freqCorrectionNorm);
            return static_cast<Type>(out * gain);
        }
        template<typename Type>
        [[nodiscard]] Type GeneratePulseThirdAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const DB gain = 1.0f) noexcept
        {
            const float phaseCorrectionNorm = indexNorm / Pi2F;
            const float freqCorrectionNorm = freqNorm / Pi2F;
            float out = GeneratePulseThirdSample<float>(indexNorm, freqNorm, index);

            out -= AntiAlliasing::GetCorrection(phaseCorrectionNorm, freqCorrectionNorm);
            out += AntiAlliasing::GetCorrection(std::fmod(phaseCorrectionNorm + 2.0f / 3.0f, 1.0f), freqCorrectionNorm);
            return static_cast<Type>(out * gain);
        }
        template<typename Type>
        [[nodiscard]] Type GeneratePulseQuarterAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const DB gain = 1.0f) noexcept
        {
            const float phaseCorrectionNorm = indexNorm / Pi2F;
            const float freqCorrectionNorm = freqNorm / Pi2F;
            float out = GeneratePulseQuarterSample<float>(indexNorm, freqNorm, index);

            out -= AntiAlliasing::GetCorrection(phaseCorrectionNorm, freqCorrectionNorm);
            out += AntiAlliasing::GetCorrection(std::fmod(phaseCorrectionNorm + 0.75f, 1.0f), freqCorrectionNorm);
            return static_cast<Type>(out * gain);
        }
        template<typename Type>
        [[nodiscard]] Type GenerateSawAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const DB gain = 1.0f) noexcept
        {
            float out = GenerateSawSample<float>(indexNorm, freqNorm, index);

            out -= AntiAlliasing::GetCorrection(indexNorm / Pi2F, freqNorm / Pi2F);
            return static_cast<Type>(out * gain);
        }


        /** @brief Waveform sample generation with input as frequency modulation */
        template<typename Type>
        [[nodiscard]] Type ModulateSineSample(const float indexNorm, const float, const std::uint32_t, const float modulation, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(indexNorm + modulation) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateCosineSample(const float indexNorm, const float, const std::uint32_t, const float modulation, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::cos(indexNorm + modulation) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateSquareSample(const float indexNorm, const float, const std::uint32_t, const float modulation, const DB gain = 1.0f) noexcept
            { return indexNorm + modulation > PiF ? gain : -gain; }
        template<typename Type>
        [[nodiscard]] Type ModulateTriangleSample(const float indexNorm, const float, const std::uint32_t, const float modulation, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((1.0f - std::acos(std::sin(indexNorm + modulation)) * static_cast<float>(M_2_PI)) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateSawSample(const float indexNorm, const float, const std::uint32_t, const float modulation, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(-std::atan(Utils::Cot(indexNorm / 2.0f + modulation)) * static_cast<float>(M_2_PI) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateNoiseSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float, const DB gain = 1.0f) noexcept
            { return GenerateNoiseSample<Type>(indexNorm, freqNorm, index, gain); }
        template<typename Type>
        [[nodiscard]] Type ModulatePulseThirdSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float modulation, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(ModulateSawSample<float>(indexNorm, freqNorm, index, modulation) + 1.0f) > 4.0f / 3.0f ? gain : -gain; }
        template<typename Type>
        [[nodiscard]] Type ModulatePulseQuarterSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float modulation, const DB gain = 1.0f) noexcept
            { return  static_cast<Type>(ModulateSawSample<float>(indexNorm, freqNorm, index, modulation) + 1.0f) > 1.5f ? gain : -gain; }

        template<typename Type>
        [[nodiscard]] Type ModulateSquareAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float modulation, const DB gain = 1.0f) noexcept
            { return GenerateSquareAnalogSample<Type>(indexNorm + modulation, freqNorm, index, gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateSawAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float modulation, const DB gain = 1.0f) noexcept
            { return GenerateSawAnalogSample<Type>(indexNorm + modulation, freqNorm, index, gain); }
        template<typename Type>
        [[nodiscard]] Type ModulatePulseThirdAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float modulation, const DB gain = 1.0f) noexcept
            { return GeneratePulseThirdAnalogSample<Type>(indexNorm + modulation, freqNorm, index, gain); }
        template<typename Type>
        [[nodiscard]] Type ModulatePulseQuarterAnalogSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const float modulation, const DB gain = 1.0f) noexcept
            { return GeneratePulseQuarterAnalogSample<Type>(indexNorm + modulation, freqNorm, index, gain); }



        /** @brief Helper used to generate a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        float WaveformGenerateHelper(const Waveform waveform, Args &&...args);

        /** @brief Helper used to modulate a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        float WaveformModulateHelper(const Waveform waveform, Args &&...args);

        /** @brief Helper used to modulate (in semitone delta) a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        float WaveformSemitoneShiftHelper(const Waveform waveform, Args &&...args);

        /** @brief Helper used to modulate (in semitone delta) a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        float WaveformModulateSemitoneShiftHelper(const Waveform waveform, Args &&...args);
    }

    /** @brief Generate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline float Generate(const Waveform waveform, Type *output, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformGenerateHelper<Accumulate>(waveform, output, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }

    template<bool Accumulate, typename Type>
    inline float Generate(const Waveform waveform, Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformGenerateHelper<Accumulate>(waveform, output, input, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline float Modulate(const Waveform waveform, Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformModulateHelper<Accumulate>(waveform, output, modulation, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }
    template<bool Accumulate, typename Type>
    inline float Modulate(const Waveform waveform, Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformModulateHelper<Accumulate>(waveform, output, input, modulation, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline float SemitoneShift(const Waveform waveform, Type *output, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformSemitoneShiftHelper<Accumulate>(waveform, output, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }
    template<bool Accumulate, typename Type>
    inline float SemitoneShift(const Waveform waveform, Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformSemitoneShiftHelper<Accumulate>(waveform, output, input, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline float ModulateSemitoneShift(const Waveform waveform, Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformModulateSemitoneShiftHelper<Accumulate>(waveform, output, modulation, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }
    template<bool Accumulate, typename Type>
    inline float ModulateSemitoneShift(const Waveform waveform, Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain = 1.0f) noexcept
        { return Internal::WaveformModulateSemitoneShiftHelper<Accumulate>(waveform, output, input, modulation, semitone, outputSize, frequencyNorm, phaseOffset, indexOffset, gain); }


    /** @brief Generate the following functions for each waveform type (ex: Sine)
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const std::size_t outputSize, const float frequencyNorm, const float phaseOffset, const DB gain = 1.0f) noexcept
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const Type *input, const std::size_t outputSize, const float frequencyNorm, const float phaseOffset, const DB gain = 1.0f) noexcept
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const Type *modulation, const std::size_t outputSize, const float frequencyNorm, const float phaseOffset, const DB gain = 1.0f) noexcept
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize, const float frequencyNorm, const float phaseOffset, const DB gain = 1.0f) noexcept
     */
    GENERATOR_REGISTER_WAVEFORM(Sine)
    GENERATOR_REGISTER_WAVEFORM(Cosine)
    GENERATOR_REGISTER_WAVEFORM(Square)
    GENERATOR_REGISTER_WAVEFORM(Triangle)
    GENERATOR_REGISTER_WAVEFORM(Saw)
    GENERATOR_REGISTER_WAVEFORM(Noise)
    GENERATOR_REGISTER_WAVEFORM(PulseThird)
    GENERATOR_REGISTER_WAVEFORM(PulseQuarter)
    GENERATOR_REGISTER_WAVEFORM(SquareAnalog)
    GENERATOR_REGISTER_WAVEFORM(SawAnalog)
    GENERATOR_REGISTER_WAVEFORM(PulseThirdAnalog)
    GENERATOR_REGISTER_WAVEFORM(PulseQuarterAnalog)
}

#undef GENERATOR_REGISTER_WAVEFORM

#include "Generator.ipp"
