/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator
 */

#pragma once

#include <Audio/Base.hpp>

#define GENERATOR_REGISTER_WAVEFORM(Name) \
    template<bool Accumulate, typename Type> \
    void Generate##Name(Type *output, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::GenerateImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void Generate##Name(Type *output, const Type *input, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::GenerateImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, input, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void Modulate##Name(Type *output, const Type *modulation, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void Modulate##Name(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void SemitoneShift##Name(Type *output, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::SemitoneShiftImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, semitone, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void SemitoneShift##Name(Type *output, const Type *input, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::SemitoneShiftImpl<Accumulate, Internal::Generate##Name##Sample<Type>>(output, input, semitone, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void ModulateSemitoneShift##Name(Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateSemitoneShiftImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, modulation, semitone, outputSize, frequencyNorm, phaseOffset, gain); } \
    template<bool Accumulate, typename Type> \
    void ModulateSemitoneShift##Name(Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize, \
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept \
        { return Internal::ModulateSemitoneShiftImpl<Accumulate, Internal::Modulate##Name##Sample<Type>>(output, input, modulation, semitone, outputSize, frequencyNorm, phaseOffset, gain); }


namespace Audio::DSP::Generator
{
    enum Waveform : std::uint8_t {
        Sine = 0,
        Cosine,
        Square,
        Triangle,
        Saw,
        Noise
    };


    namespace Internal
    {
        /** @brief Generate implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void GenerateImpl(Type *output, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void GenerateImpl(Type *output, const Type *input, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;

        /** @brief Modulate implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void ModulateImpl(Type *output, const Type *modulation, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;

        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void ModulateImpl(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;

        /** @brief Modulate with semitone delta implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void SemitoneShiftImpl(Type *output, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;

        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void SemitoneShiftImpl(Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;

        /** @brief Modulate with semitone delta implementation */
        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void ModulateSemitoneShiftImpl(Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;

        template<bool Accumulate, auto ComputeFunc, typename Type>
        inline void ModulateSemitoneShiftImpl(Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept;


        /** @brief Waveform sample generation */
        template<typename Type>
        [[nodiscard]] Type GenerateSineSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(static_cast<float>(index) * frequencyNorm) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateCosineSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::cos(static_cast<float>(index) * frequencyNorm) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateSquareSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return GenerateSineSample<Type>(frequencyNorm, index) > 0 ? gain : -gain; }
        template<typename Type>
        [[nodiscard]] Type GenerateTriangleSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((1.0f - std::acos(std::cos(2.0f * static_cast<float>(index) * frequencyNorm)) * static_cast<float>(M_2_PI)) * gain); }
        template<typename Type>
        [[nodiscard]] Type GenerateSawSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(-std::atan(Utils::cot(static_cast<float>(index) * frequencyNorm)) * static_cast<float>(M_2_PI) * gain); }

        template<typename Type>
        [[nodiscard]] Type GenerateNoiseSample(const float, const std::size_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(gain * static_cast<float>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max())); }


        /** @brief Waveform sample generation with input as frequency modulation */
        template<typename Type>
        [[nodiscard]] Type ModulateSineSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(static_cast<float>(index) * frequencyNorm + modulation) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateCosineSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::cos(static_cast<float>(index) * frequencyNorm + modulation) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateSquareSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(static_cast<float>(index) * frequencyNorm + modulation) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateTriangleSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((1.0f - std::acos(std::cos(2.0f * static_cast<float>(index) * frequencyNorm + modulation)) * static_cast<float>(M_2_PI)) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateSawSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(-std::atan(Utils::cot(static_cast<float>(index) * frequencyNorm + modulation)) * static_cast<float>(M_2_PI) * gain); }
        template<typename Type>
        [[nodiscard]] Type ModulateNoiseSample(const float, const float, const std::size_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(gain * static_cast<float>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max())); }


        /** @brief Helper used to generate a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        void WaveformGenerateHelper(const Waveform waveform, Args &&...args);

        /** @brief Helper used to modulate a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        void WaveformModulateHelper(const Waveform waveform, Args &&...args);

        /** @brief Helper used to modulate (in semitone delta) a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        void WaveformSemitoneShiftHelper(const Waveform waveform, Args &&...args);

        /** @brief Helper used to modulate (in semitone delta) a waveform function using runtime specialization */
        template<bool Accumulate, typename ...Args>
        void WaveformModulateSemitoneShiftHelper(const Waveform waveform, Args &&...args);
    }

    /** @brief Generate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline void Generate(const Waveform waveform, Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformGenerateHelper<Accumulate>(waveform, output, outputSize, frequencyNorm, phaseOffset, gain); }

    template<bool Accumulate, typename Type>
    inline void Generate(const Waveform waveform, Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformGenerateHelper<Accumulate>(waveform, output, input, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline void Modulate(const Waveform waveform, Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformModulateHelper<Accumulate>(waveform, output, modulation, outputSize, frequencyNorm, phaseOffset, gain); }
    template<bool Accumulate, typename Type>
    inline void Modulate(const Waveform waveform, Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformModulateHelper<Accumulate>(waveform, output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline void SemitoneShift(const Waveform waveform, Type *output, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformSemitoneShiftHelper<Accumulate>(waveform, output, semitone, outputSize, frequencyNorm, phaseOffset, gain); }
    template<bool Accumulate, typename Type>
    inline void SemitoneShift(const Waveform waveform, Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformSemitoneShiftHelper<Accumulate>(waveform, output, input, semitone, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Modulate a waveform using runtime specialization */
    template<bool Accumulate, typename Type>
    inline void ModulateSemitoneShift(const Waveform waveform, Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformModulateSemitoneShiftHelper<Accumulate>(waveform, output, modulation, semitone, outputSize, frequencyNorm, phaseOffset, gain); }
    template<bool Accumulate, typename Type>
    inline void ModulateSemitoneShift(const Waveform waveform, Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
        { return Internal::WaveformModulateSemitoneShiftHelper<Accumulate>(waveform, output, input, modulation, semitone, outputSize, frequencyNorm, phaseOffset, gain); }


    /** @brief Generate the following functions for each waveform type (ex: Sine)
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const std::size_t outputSize, const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const Type *input, const std::size_t outputSize, const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const Type *modulation, const std::size_t outputSize, const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
     *
     *  template<bool Accumulate, typename Type>
     *  void GenerateSine(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize, const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
     */
    GENERATOR_REGISTER_WAVEFORM(Sine)
    GENERATOR_REGISTER_WAVEFORM(Cosine)
    GENERATOR_REGISTER_WAVEFORM(Square)
    GENERATOR_REGISTER_WAVEFORM(Triangle)
    GENERATOR_REGISTER_WAVEFORM(Saw)
    GENERATOR_REGISTER_WAVEFORM(Noise)
}

#undef GENERATOR_REGISTER_WAVEFORM

#include "Generator.ipp"
