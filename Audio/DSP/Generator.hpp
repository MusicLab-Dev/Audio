/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator
 */

#pragma once

#include <Audio/Base.hpp>

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
        template<bool Accumulate, auto ComputeFunc, typename Type>
        void GenerateWaveformImpl(Type *output, const Type *input, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;

        template<bool Accumulate, auto ComputeFunc, typename Type>
        void GenerateWaveformImpl(Type *output, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;

        template<bool Accumulate, auto ComputeFunc, typename Type>
        void GenerateModulateWaveformImpl(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;

        template<bool Accumulate, auto ComputeFunc, typename Type>
        void GenerateModulateWaveformImpl(Type *output, const Type *modulation, const std::size_t outputSize,
                const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;


        /** @brief Waveform generation */
        template<typename Type>
        Type GenerateSineSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(static_cast<float>(index) * frequencyNorm) * gain); }

        template<typename Type>
        Type GenerateCosineSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::cos(static_cast<float>(index) * frequencyNorm) * gain); }

        template<typename Type>
        Type GenerateSquareSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return GenerateSineSample<Type>(frequencyNorm, index) > 0 ? gain : -gain; }

        template<typename Type>
        Type GenerateTriangleSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((1.0f - std::acos(std::cos(2.0f * static_cast<float>(index) * frequencyNorm)) * static_cast<float>(M_2_PI)) * gain); }

        template<typename Type>
        Type GenerateSawSample(const float frequencyNorm, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(-std::atan(Utils::cot(static_cast<float>(index) * frequencyNorm)) * static_cast<float>(M_2_PI) * gain); }

        template<typename Type>
        Type GenerateNoiseSample(const float, const std::size_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(gain * static_cast<float>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max())); }

        /** @brief Waveform generation with input as frequency modulation */
        template<typename Type>
        Type GenerateModulateSineSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(static_cast<float>(index) * frequencyNorm + modulation) * gain); }

        template<typename Type>
        Type GenerateModulateCosineSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::cos(static_cast<float>(index) * frequencyNorm + modulation) * gain); }

        template<typename Type>
        Type GenerateModulateSquareSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(std::sin(static_cast<float>(index) * frequencyNorm + modulation) * gain); }

        template<typename Type>
        Type GenerateModulateTriangleSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>((1.0f - std::acos(std::cos(2.0f * static_cast<float>(index) * frequencyNorm + modulation)) * static_cast<float>(M_2_PI)) * gain); }

        template<typename Type>
        Type GenerateModulateSawSample(const float frequencyNorm, const float modulation, const std::size_t index, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(-std::atan(Utils::cot(static_cast<float>(index) * frequencyNorm + modulation)) * static_cast<float>(M_2_PI) * gain); }

        template<typename Type>
        Type GenerateModulateNoiseSample(const float, const float, const std::size_t, const DB gain = 1.0f) noexcept
            { return static_cast<Type>(gain * static_cast<float>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max())); }

    }


    /** @brief Runtime waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateWaveform(Waveform waveform, Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;

    /** @brief Runtime waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateWaveform(Waveform waveform, Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;

    /** @brief Runtime waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateWaveform(Waveform waveform, Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;

    /** @brief Runtime waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateModulateWaveform(Waveform waveform, Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept;



    /** @brief Sine waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateSine(Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateSineSample<Type>>(output, input, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Sine waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateSine(Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateSineSample<Type>>(output, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Sine waveform generation & modulation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateSine(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateSineSample<Type>>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Sine waveform generation & modulation */
    template<bool Accumulate, typename Type>
    void GenerateModulateSine(Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateSineSample<Type>>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain); }


    /** @brief Cosine waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateCosine(Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateCosineSample<Type>>(output, input, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Cosine waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateCosine(Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateCosineSample<Type>>(output, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Cosine waveform generation & modulation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateCosine(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateCosineSample<Type>>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Cosine waveform generation & modulation */
    template<bool Accumulate, typename Type>
    void GenerateModulateCosine(Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateCosineSample<Type>>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain); }


    /** @brief Square waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateSquare(Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateSquareSample<Type>>(output, input, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Square waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateSquare(Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateSquareSample<Type>>(output, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Square waveform generation & modulation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateSquare(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateSquareSample<Type>>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Square waveform generation & modulation */
    template<bool Accumulate, typename Type>
    void GenerateModulateSquare(Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateSquareSample<Type>>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain); }


    /** @brief Triangle waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateTriangle(Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
    { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateTriangleSample<Type>>(output, input, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }

    /** @brief Triangle waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateTriangle(Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateTriangleSample<Type>>(output, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }

    /** @brief Triangle waveform generation & modulation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateTriangle(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
    { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateTriangleSample<Type>>(output, input, modulation, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }

    /** @brief Triangle waveform generation & modulation */
    template<bool Accumulate, typename Type>
    void GenerateModulateTriangle(Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateTriangleSample<Type>>(output, modulation, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }


    /** @brief Saw waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateSaw(Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateSawSample<Type>>(output, input, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }

    /** @brief Saw waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateSaw(Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateSawSample<Type>>(output, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }

    /** @brief Saw waveform generation & modulation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateSaw(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateSawSample<Type>>(output, input, modulation, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }

    /** @brief Saw waveform generation & modulation */
    template<bool Accumulate, typename Type>
    void GenerateModulateSaw(Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateSawSample<Type>>(output, modulation, outputSize, frequencyNorm / 2.0f, phaseOffset, gain); }


    /** @brief Noise waveform generation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateNoise(Type *output, const Type *input, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateNoiseSample<Type>>(output, input, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Noise waveform generation */
    template<bool Accumulate, typename Type>
    void GenerateNoise(Type *output, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateWaveformImpl<Accumulate, Internal::GenerateNoiseSample<Type>>(output, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Noise waveform generation & modulation using an input as an envelope */
    template<bool Accumulate, typename Type>
    void GenerateModulateNoise(Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateNoiseSample<Type>>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain); }

    /** @brief Noise waveform generation & modulation */
    template<bool Accumulate, typename Type>
    void GenerateModulateNoise(Type *output, const Type *modulation, const std::size_t outputSize,
            const float frequencyNorm, const std::uint32_t phaseOffset = 0u, const DB gain = 1.0f) noexcept
        { return Internal::GenerateModulateWaveformImpl<Accumulate, Internal::GenerateModulateNoiseSample<Type>>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain); }
};

#include "Generator.ipp"
