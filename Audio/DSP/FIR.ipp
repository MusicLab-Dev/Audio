/**
 * @ Author: Pierre Veysseyre
 * @ Description: FIR.ipp
 */

#include <Audio/Math.hpp>

#include <Core/SPSCQueue.hpp>

inline std::size_t Audio::DSP::FIR::GetResampleSize(const std::size_t size, const std::size_t interpFactor, const std::size_t decimFactor) noexcept
{
    return (size * interpFactor) / decimFactor;
}

template<unsigned ProcessRate, typename Type>
void shiftCoefficients(Type *filterCoefficients, const std::size_t filterSize, const std::size_t interpRate)
{

}

template<unsigned ProcessRate, typename Type>
inline Audio::DSP::FIR::VoidType<Type>
Audio::DSP::FIR::Interpolate(
        Type *producedData, const Type *input,
        const Type *filterCoefs, const std::size_t interpFactor,
        const std::size_t zeroPadBegin, const std::size_t zeroPadEnd)
{
    const auto outSize = interpFactor - zeroPadEnd;
    // std::cout << "outSize: " << outSize << ", zeroPadBegin: " << zeroPadBegin << ", zeroPadEnd: " << zeroPadEnd << std::endl;
    for (auto i = 0u; i < outSize; ++i) {
        producedData[outSize - 1 - i] = 0.0;
        // std::cout << "\ni: " << i << std::endl;
        // std::cout << "-outIdx: " << outSize - 1 - i << std::endl;
        for (auto k = zeroPadBegin; k < ProcessRate; ++k) {
            // std::cout << "  --k: " << k << std::endl;
            // std::cout << "  --  inputIdx: " << k - zeroPadBegin << std::endl;
            // std::cout << "  --  filterIdx: " << (i + k * interpFactor) << std::endl;
            producedData[outSize - 1 - i] += (input[k - zeroPadBegin] * filterCoefs[i + (k * interpFactor)]) * (interpFactor * 0.707) / 2;
        }
    }
}

// template<typename Type>
// std::size_t decimate(Core::SPSCQueue<Type> &producedData, Type *output, const std::size_t decimFactor)
// {
    // producedData[]
//     return producedData.popRange(output);
// }

template<typename Type>
void popRange(std::vector<Type> &producedCache, Type *output, const std::size_t decimFactor)
{

}

template<unsigned ProcessRate, typename Type>
void interpolateX()
{

}

// template<unsigned ProcessRate, typename Type>
// inline Audio::DSP::FIR::VoidType<Type>
//     Audio::DSP::FIR::ResampleX(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor) noexcept
// {
//     const std::size_t filterSize = interpFactor * ProcessRate;
//     const FilterSpecs filterSpecs {
//         FilterType::LowPass,
//         WindowType::Hanning,
//         filterSize,
//         static_cast<float>(inputSampleRate),
//         { inputSampleRate / static_cast<float>(interpFactor) / 2, 0.0 }
//     };
//     std::vector<Type> filterCoefs(filterSize);
//     WindowMaker::GenerateFilterCoefficients(filterSpecs.windowType, filterSize, filterCoefs.data());
//     DesignFilter(filterSpecs, filterCoefs.data(), filterSize);
//     // shiftCoefficients<ProcessRate>(filterCoefs.data(), filterSize, interpFactor);

//     const std::size_t totalBlock = std::ceil(inputSize / static_cast<float>(ProcessRate));
//     const std::size_t lastBlockSize = inputSize % ProcessRate;
//     std::vector<Type> producedCache;
//     producedCache.reserve(std::max(interpFactor, decimFactor) * ProcessRate * 3);
//     // std::vector<Type> interpolateData(interpFactor);
//     // std::vector<Type> decimData(decimFactor);

//     // std::cout << "totalBlock: " << totalBlock << std::endl;
//     // std::cout << "lastBlockSize: " << lastBlockSize << std::endl;
//     // std::cout << "inputSize: " << inputSize << std::endl;
//     // std::cout << "filterSize: " << filterSize << std::endl;
//     // std::cout << "totalBlock: " << totalBlock << std::endl;

//     const auto out = output;
//     // std::cout << "::" << output << std::endl;
//     auto p = 0u;
//     auto c = 0u;
//     auto failed = 0u;
//     // for (auto i = 0u; i < ProcessRate - 1; ++i) {
//     //     Interpolate<ProcessRate>(interpolateData, input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
//     //     auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
//     //     p += produced;
//     //     while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
//     //         // std::cout << "consumed: " << std::endl;
//     //         *output = decimData[0];
//     //         ++output;
//     //         c += decimFactor;
//     //     }
//     // }
//     std::cout << "S::: " << producedCache.size() << std::endl;
//     std::cout << "C::: " << producedCache.capacity() << std::endl;
//     // return;
//     for (auto i = ProcessRate - 1; i < inputSize; ++i) {
//         const std::size_t idx = i * ProcessRate;
//         std::cout << "ii::: " << idx << std::endl;
//         std::cout << "S::: " << producedCache.size() << std::endl;
//         // Produce interpFactor of samples
//         Interpolate<ProcessRate>(producedCache.data() + producedCache.size(), input + i, filterCoefs.data(), interpFactor);
//         p += interpFactor;
//         for (auto d = 0u; (producedCache.size() / decimFactor); ++d) {
//             std::cout << "Consume " << std::endl;
//             *output = producedCache[0];
//             ++output;
//             c += decimFactor;
//             producedCache.erase(producedCache.begin(), producedCache.begin() + decimFactor);
//         }

//         std::cout << "::" << output - out << std::endl;
//         // break;
//     }
//     std::cout << "produced total: " << p << std::endl;
//     std::cout << "consumed total: " << c << std::endl;
//     std::cout << "::" << output << std::endl;
// }

// template<unsigned ProcessRate, typename Type>
// inline Audio::DSP::FIR::VoidType<Type>
//     Audio::DSP::FIR::ResampleSTD(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor) noexcept
// {
//     const std::size_t filterSize = interpFactor * ProcessRate;
//     const FilterSpecs filterSpecs {
//         FilterType::LowPass,
//         WindowType::Hanning,
//         filterSize,
//         static_cast<float>(inputSampleRate),
//         { inputSampleRate / static_cast<float>(interpFactor) / 2, 0.0 }
//     };
//     std::vector<Type> filterCoefs(filterSize);
//     WindowMaker::GenerateFilterCoefficients(filterSpecs.windowType, filterSize, filterCoefs.data());
//     DesignFilter(filterSpecs, filterCoefs.data(), filterSize);
//     // shiftCoefficients<ProcessRate>(filterCoefs.data(), filterSize, interpFactor);

//     const std::size_t totalBlock = std::ceil(inputSize / static_cast<float>(ProcessRate));
//     const std::size_t lastBlockSize = inputSize % ProcessRate;
//     std::queue<Type> producedCache (std::max(interpFactor, decimFactor) * ProcessRate * 3);
//     std::vector<Type> interpolateData(interpFactor);
//     std::vector<Type> decimData(decimFactor);

//     // std::cout << "totalBlock: " << totalBlock << std::endl;
//     // std::cout << "lastBlockSize: " << lastBlockSize << std::endl;
//     // std::cout << "inputSize: " << inputSize << std::endl;
//     // std::cout << "filterSize: " << filterSize << std::endl;
//     // std::cout << "totalBlock: " << totalBlock << std::endl;


//     // return;
//     const auto out = output;
//     // std::cout << "::" << output << std::endl;
//     auto p = 0u;
//     auto c = 0u;
//     auto failed = 0u;
//     for (auto i = 0u; i < ProcessRate - 1; ++i) {
//         Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
//         auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
//         p += produced;
//         while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
//             // std::cout << "consumed: " << std::endl;
//             *output = decimData[0];
//             ++output;
//             c += decimFactor;
//         }
//     }
//     // return;
//     for (auto i = ProcessRate - 1; i < inputSize; ++i) {
//         const std::size_t idx = i * ProcessRate;
//         // std::cout << "ii::: " << idx << std::endl;
//         // Produce interpFactor of samples
//         Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor);
//         auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
//         p += produced;
//         // if (produced != interpFactor)
//             // std::cout << "failed production: " << produced << "/" << filterSize << std::endl;
//         // else
//             // std::cout << "produced: " << produced << std::endl;
//         // If possible, consume decimFactor samples
//         while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
//             // std::cout << "consumed: " << std::endl;
//             *output = decimData[0];
//             ++output;
//             c += decimFactor;
//         }
//         // break;
//     }
//     // std::cout << "produced total: " << p << std::endl;
//     // std::cout << "consumed total: " << c << std::endl;
//     // std::cout << "::" << output << std::endl;
//     // std::cout << "::" << output - out << std::endl;
// }


template<unsigned ProcessRate, bool Accumulate, typename Type>
inline Audio::DSP::FIR::VoidType<Type>
    Audio::DSP::FIR::Resample(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor, const std::size_t offset) noexcept
{
    const std::size_t filterSize = interpFactor * ProcessRate;
    const FilterSpecs filterSpecs {
        FilterType::LowPass,
        WindowType::Hanning,
        filterSize,
        static_cast<float>(inputSampleRate),
        { inputSampleRate / 2 / static_cast<float>(std::max(interpFactor, decimFactor)), 0.0 }
    };
    std::vector<Type> filterCoefs(filterSize);
    WindowMaker::GenerateFilterCoefficients(filterSpecs.windowType, filterSize, filterCoefs.data());
    DesignFilter(filterSpecs, filterCoefs.data(), filterSize);
    // shiftCoefficients<ProcessRate>(filterCoefs.data(), filterSize, interpFactor);

    const std::size_t totalBlock = std::ceil(inputSize / static_cast<float>(ProcessRate));
    const std::size_t lastBlockSize = inputSize % ProcessRate;
    Core::SPSCQueue<Type> producedCache { std::max(interpFactor, decimFactor) * 2 * ProcessRate * 3 };
    std::vector<Type> interpolateData(interpFactor);
    std::vector<Type> decimData(decimFactor);

    // std::cout << "totalBlock: " << totalBlock << std::endl;
    // std::cout << "lastBlockSize: " << lastBlockSize << std::endl;
    // std::cout << "inputSize: " << inputSize << std::endl;
    // std::cout << "filterSize: " << filterSize << std::endl;
    // std::cout << "totalBlock: " << totalBlock << std::endl;


    // return;
    const auto out = output;
    // std::cout << "::" << output << std::endl;
    auto pBegin = 0u;
    auto pBody = 0u;
    auto cBegin = 0u;
    auto cBody = 0u;
    auto failed = 0u;
    for (auto i = offset; i < ProcessRate - 1; ++i) {
        // std::cout << "  ii: " << i << std::endl;
        Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
        auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
        pBegin += produced;
        while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
            // std::cout << "consumed: " << std::endl;
            if constexpr (Accumulate)
                *output += decimData[0];
            else
                *output = decimData[0];
            ++output;
            cBegin += 1;
        }
    }
    // return;
    const auto start = offset ? offset : offset + ProcessRate - 1;
    for (auto i = start; i < inputSize; ++i) {
        const std::size_t idx = i * ProcessRate;
        // std::cout << "  ii::: " << i << std::endl;
        // Produce interpFactor of samples
        Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor);
        auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
        pBody += produced;
        // if (produced != interpFactor)
            // std::cout << "failed production: " << produced << "/" << filterSize << std::endl;
        // else
            // std::cout << "produced: " << produced << std::endl;
        // If possible, consume decimFactor samples
        while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
            // std::cout << "consumed: " << std::endl;
            if constexpr (Accumulate)
                *output += decimData[0];
            else
                *output = decimData[0];
            ++output;
            cBody += 1;
        }
        // break;
    }
    // std::cout << "Begin: " << pBegin << ", " << cBegin << std::endl;
    // std::cout << "Body: " << pBody << ", " << cBody << std::endl;
    // std::cout << "all: " << pBegin + pBody << ", " << cBegin + cBody << std::endl;
    // std::cout << ":: " << inputSize - offset << std::endl;
    // std::cout << std::endl;
    // std::cout << "produced total: " << p << std::endl;
    // std::cout << "consumed total: " << c << std::endl;
    // std::cout << "::" << output << std::endl;
    // std::cout << "::" << output - out << std::endl;
}


template<unsigned ProcessRate, bool Accumulate, typename Type>
inline Audio::DSP::FIR::VoidType<Type>
Audio::DSP::FIR::ResampleOctave(
        const Type *input, Type *output,
        const std::size_t inputSize, const std::size_t inputSampleRate,
        const std::int8_t nOctave,
        const std::size_t offset) noexcept
{
    const auto nOctaveDt = std::abs(nOctave);

    if (nOctave > 0)
        return DecimateOctaveImpl<ProcessRate>(input, output, inputSize, inputSampleRate, nOctaveDt, offset);
    else
        return InterpolateOctaveImpl<ProcessRate>(input, output, inputSize, inputSampleRate, nOctaveDt, offset);
}

template<unsigned ProcessRate, typename Type>
inline Audio::DSP::FIR::VoidType<Type>
Audio::DSP::FIR::InterpolateOctaveImpl(
        const Type *input, Type *output,
        const std::size_t inputSize, const std::size_t inputSampleRate,
        const std::uint8_t nOctave,
        const std::size_t offset) noexcept
{
    const std::size_t filterSize = nOctave * ProcessRate;
    const FilterSpecs filterSpecs {
        FilterType::LowPass,
        WindowType::Hanning,
        filterSize,
        static_cast<float>(inputSampleRate),
        { inputSampleRate / static_cast<float>(std::pow(2.f, nOctave)), 0.f }
    };
    std::vector<Type> filterCoefs(filterSize);
    WindowMaker::GenerateFilterCoefficients(filterSpecs.windowType, filterSize, filterCoefs.data());
    DesignFilter(filterSpecs, filterCoefs.data(), filterSize);
    // shiftCoefficients<ProcessRate>(filterCoefs.data(), filterSize, interpFactor);

    const std::size_t totalBlock = std::ceil(inputSize / static_cast<float>(ProcessRate));
    const std::size_t lastBlockSize = inputSize % ProcessRate;


    // // std::cout << "totalBlock: " << totalBlock << std::endl;
    // // std::cout << "lastBlockSize: " << lastBlockSize << std::endl;
    // // std::cout << "inputSize: " << inputSize << std::endl;
    // // std::cout << "filterSize: " << filterSize << std::endl;
    // // std::cout << "totalBlock: " << totalBlock << std::endl;


    // // return;
    // const auto out = output;
    // // std::cout << "::" << output << std::endl;
    // auto pBegin = 0u;
    // auto pBody = 0u;
    // auto cBegin = 0u;
    // auto cBody = 0u;
    // auto failed = 0u;
    // for (auto i = offset; i < ProcessRate - 1; ++i) {
    //     // std::cout << "  ii: " << i << std::endl;
    //     Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
    //     auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
    //     pBegin += produced;
    //     while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
    //         // std::cout << "consumed: " << std::endl;
    //         if constexpr (Accumulate)
    //             *output += decimData[0];
    //         else
    //             *output = decimData[0];
    //         ++output;
    //         cBegin += 1;
    //     }
    // }
    // // return;
    // const auto start = offset ? offset : offset + ProcessRate - 1;
    // for (auto i = start; i < inputSize; ++i) {
    //     const std::size_t idx = i * ProcessRate;
    //     // std::cout << "  ii::: " << i << std::endl;
    //     // Produce interpFactor of samples
    //     Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor);
    //     auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
    //     pBody += produced;
    //     // if (produced != interpFactor)
    //         // std::cout << "failed production: " << produced << "/" << filterSize << std::endl;
    //     // else
    //         // std::cout << "produced: " << produced << std::endl;
    //     // If possible, consume decimFactor samples
    //     while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
    //         // std::cout << "consumed: " << std::endl;
    //         if constexpr (Accumulate)
    //             *output += decimData[0];
    //         else
    //             *output = decimData[0];
    //         ++output;
    //         cBody += 1;
    //     }
    //     // break;
    // }

}

template<unsigned ProcessRate, typename Type>
inline Audio::DSP::FIR::VoidType<Type>
Audio::DSP::FIR::DecimateOctaveImpl(
        const Type *input, Type *output,
        const std::size_t inputSize, const std::size_t inputSampleRate,
        const std::uint8_t nOctave,
        const std::size_t offset) noexcept
{
    // const std::size_t filterSize = (nOctave > 0 ?  :  ) * ProcessRate;
    // const FilterSpecs filterSpecs {
    //     FilterType::LowPass,
    //     WindowType::Hanning,
    //     filterSize,
    //     static_cast<float>(inputSampleRate),
    //     { inputSampleRate / std::pow(2, std::abs(nOctave)), 0.0 }
    // };
    // std::vector<Type> filterCoefs(filterSize);
    // WindowMaker::GenerateFilterCoefficients(filterSpecs.windowType, filterSize, filterCoefs.data());
    // DesignFilter(filterSpecs, filterCoefs.data(), filterSize);
    // // shiftCoefficients<ProcessRate>(filterCoefs.data(), filterSize, interpFactor);

    // const std::size_t totalBlock = std::ceil(inputSize / static_cast<float>(ProcessRate));
    // const std::size_t lastBlockSize = inputSize % ProcessRate;
    // Core::SPSCQueue<Type> producedCache { std::max(interpFactor, decimFactor) * 2 * ProcessRate * 3 };
    // std::vector<Type> interpolateData(interpFactor);
    // std::vector<Type> decimData(decimFactor);

    // // std::cout << "totalBlock: " << totalBlock << std::endl;
    // // std::cout << "lastBlockSize: " << lastBlockSize << std::endl;
    // // std::cout << "inputSize: " << inputSize << std::endl;
    // // std::cout << "filterSize: " << filterSize << std::endl;
    // // std::cout << "totalBlock: " << totalBlock << std::endl;


    // // return;
    // const auto out = output;
    // // std::cout << "::" << output << std::endl;
    // auto pBegin = 0u;
    // auto pBody = 0u;
    // auto cBegin = 0u;
    // auto cBody = 0u;
    // auto failed = 0u;
    // for (auto i = offset; i < ProcessRate - 1; ++i) {
    //     // std::cout << "  ii: " << i << std::endl;
    //     Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
    //     auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
    //     pBegin += produced;
    //     while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
    //         // std::cout << "consumed: " << std::endl;
    //         if constexpr (Accumulate)
    //             *output += decimData[0];
    //         else
    //             *output = decimData[0];
    //         ++output;
    //         cBegin += 1;
    //     }
    // }
    // // return;
    // const auto start = offset ? offset : offset + ProcessRate - 1;
    // for (auto i = start; i < inputSize; ++i) {
    //     const std::size_t idx = i * ProcessRate;
    //     // std::cout << "  ii::: " << i << std::endl;
    //     // Produce interpFactor of samples
    //     Interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor);
    //     auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
    //     pBody += produced;
    //     // if (produced != interpFactor)
    //         // std::cout << "failed production: " << produced << "/" << filterSize << std::endl;
    //     // else
    //         // std::cout << "produced: " << produced << std::endl;
    //     // If possible, consume decimFactor samples
    //     while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
    //         // std::cout << "consumed: " << std::endl;
    //         if constexpr (Accumulate)
    //             *output += decimData[0];
    //         else
    //             *output = decimData[0];
    //         ++output;
    //         cBody += 1;
    //     }
    //     // break;
    // }
}

template<
    bool RemoveDelay = true,
    bool ProcessFirstChunk = true,
    typename Type>
inline Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::Filter(
        const Audio::DSP::FilterSpecs specs,
        const Type *input, Type *output,
        const std::size_t inputSize) noexcept
{
    /** @todo Check if inputSize < windowSize ! */
    const auto windowSize = specs.windowSize;
    std::vector<float> windowFilter(windowSize);
    Audio::DSP::WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
    Audio::DSP::FIR::DesignFilter(specs, windowFilter.data(), windowSize);

    // This remove delay due to the convolution
    if constexpr (RemoveDelay) {
        // Highest part of the input
        for (auto i = 0u; i < inputSize - windowSize / 2 + 1; ++i) {
            const auto idx = inputSize - i - 1u;
            output[idx] = FilterImpl(input + idx - windowSize / 2, windowFilter.data(), windowSize, 0u);
        }
        // Lower part of the input
        if constexpr (ProcessFirstChunk) {
            for (auto i = inputSize - windowSize + 1; i < inputSize - windowSize / 2; ++i) {
                const auto idx = inputSize - i - 1u;
                output[idx - windowSize / 2] = FilterImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
            }
        }
    } else {
        // Hihest part of the input
        for (auto i = 0u; i < inputSize - windowSize + 1; ++i) {
            const auto idx = inputSize - i - 1u;
            // std::cout << "i: " << i << ", idx: " << idx << ", zeroPad: " << 0 << ", inOffset" << (idx - windowSize + 1) << std::endl;
            output[idx] = FilterImpl(input + idx - windowSize, windowFilter.data(), windowSize, 0u);
        }
        // Lower part of the input
        if constexpr (ProcessFirstChunk) {
            for (auto i = inputSize - windowSize + 1; i < inputSize; ++i) {
                const auto idx = inputSize - i - 1u;
                output[idx] = FilterImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
            }
        }
    }

    /** @brief Default version, maybe slower */
    // for (auto i = 0u; i < inputSize; ++i) {
    //     const auto idx = inputSize - i - 1u;
    //     const auto dt = (idx < windowSize ? windowSize - 1 - idx : 0u);
    //     const auto inputOffset = idx > windowSize - 1 ? idx - windowSize + 1 : 0;
    //     std::cout << i << ", " << idx << ", " << dt << ", " << inputOffset << std::endl;
    //     output[idx] = FilterImpl(input + inputOffset, coefs, windowSize, dt);
    // }

    // Normalize output
    // std::for_each(output, output + inputSize, [](float &value) {
        // value /= std::sqrt(2.0);
    // });
}

template<
    bool RemoveDelay = true,
    bool ProcessFirstChunk = true,
    typename Type>
inline Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::FilterLastInput(
        const Audio::DSP::FilterSpecs specs,
        const Type *input, Type *output,
        const std::size_t inputSize,
        const Type *lastInput, const std::size_t lastInputSize) noexcept
{
    const auto windowSize = specs.windowSize;
    std::vector<float> windowFilter(windowSize);
    Audio::DSP::WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
    Audio::DSP::FIR::DesignFilter(specs, windowFilter.data(), windowSize);

    if constexpr (RemoveDelay) {
        // Highest part of the input
        for (auto i = 0u; i < inputSize - windowSize / 2 + 1; ++i) {
            const auto idx = inputSize - i - 1u;
            output[idx] = FilterImpl(input + idx - windowSize / 2, windowFilter.data(), windowSize, 0u);
        }
        // Lower part of the input
        if constexpr (ProcessFirstChunk) {
            const auto lastInputOffset = (lastInputSize < windowSize) ? 0u : (lastInputSize - (windowSize - 1) - 1);
            for (auto i = inputSize - windowSize + 1; i < inputSize - windowSize / 2; ++i) {
                const auto idx = inputSize - i - 1u;
                output[idx - windowSize / 2] = FilterLastInputImpl(input, lastInput + lastInputOffset, windowFilter.data(), windowSize, windowSize - 1 - idx, lastInputSize - lastInputOffset);
                // output[idx - windowSize / 2] = FilterImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
            }
        }
    } else {
        // Hihest part of the input
        for (auto i = 0u; i < inputSize - windowSize + 1; ++i) {
            const auto idx = inputSize - i - 1u;
            // std::cout << "i: " << i << ", idx: " << idx << ", zeroPad: " << 0 << ", inOffset" << (idx - windowSize + 1) << std::endl;
            output[idx] = FilterImpl(input + idx - windowSize, windowFilter.data(), windowSize, 0u);
        }
        // Lower part of the input
        if constexpr (ProcessFirstChunk) {
            const auto lastInputOffset = (lastInputSize < windowSize) ? 0u : (lastInputSize - (windowSize - 1));
            for (auto i = inputSize - windowSize + 1; i < inputSize; ++i) {
                const auto idx = inputSize - i - 1u;
                output[idx] = FilterLastInputImpl(input, lastInput, windowFilter.data(), windowSize, windowSize - 1 - idx, lastInputSize - lastInputOffset);
                // output[idx] = FilterImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
            }
        }
    }

    // Normalize output
    // std::for_each(output, output + inputSize, [](float &value){
        // value /= std::sqrt(2.0);
    // });

}

template<typename Type>
inline Audio::DSP::FIR::ProcessType<Type>
    Audio::DSP::FIR::FilterImpl(
            const Type *input, const Type *window,
            const std::size_t processSize, const std::size_t zeroPad) noexcept
{
    Type sample { 0.0 };

    // Input convultion
    for (auto i = zeroPad; i < processSize; ++i) {
        const auto idx = processSize - i - 1;
        sample += (input[idx] * window[idx + zeroPad]);
    }
    return sample;
}

template<typename Type>
inline Audio::DSP::FIR::ProcessType<Type>
    Audio::DSP::FIR::FilterLastInputImpl(
            const Type *input, const Type *lastInput,
            const Type *window, const std::size_t processSize,
            const std::size_t zeroPad, const std::size_t zeroPadLastInput) noexcept
{
    Type sample { 0.0 };

    // Input convolution
    for (auto i = zeroPad; i < processSize; ++i) {
        const auto idx = processSize - i - 1;
        sample += (input[idx] * window[idx + zeroPad]);
    }

    // Last input convolution
    for (auto i = 0; i < zeroPad; ++i) {
        const auto idx = processSize - i - 1;
        sample += (lastInput[idx] * window[zeroPad - i - 1]);
    }
    return sample;
}


inline void Audio::DSP::FIR::DesignFilter(const FilterSpecs specs, float *windowCoefficients, const std::size_t windowSize, const bool centered) noexcept
{
    const float cutoffRate = specs.cutoffs[0] / static_cast<float>(specs.sampleRate);
    switch (specs.filterType) {
    case FilterType::LowPass:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate, centered);

    default:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate, centered);
    }
}

inline void Audio::DSP::FIR::DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept
{
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= (realRate * Utils::sinc<true>(idx * realRate));
        ++windowCoefficients;
    }
}