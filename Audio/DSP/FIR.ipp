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
void interpolate(
        Type *producedData, const Type *input,
        const Type *filterCoefs, const std::size_t interpFactor,
        const std::size_t zeroPadBegin = 0u, const std::size_t zeroPadEnd = 0u)
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
//     //     interpolate<ProcessRate>(interpolateData, input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
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
//         interpolate<ProcessRate>(producedCache.data() + producedCache.size(), input + i, filterCoefs.data(), interpFactor);
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

#include <queue>

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
//         interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
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
//         interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor);
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


template<unsigned ProcessRate, typename Type>
inline Audio::DSP::FIR::VoidType<Type>
    Audio::DSP::FIR::Resample(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor) noexcept
{
    const std::size_t filterSize = interpFactor * ProcessRate;
    const FilterSpecs filterSpecs {
        FilterType::LowPass,
        WindowType::Hanning,
        filterSize,
        static_cast<float>(inputSampleRate),
        { inputSampleRate / static_cast<float>(std::max(interpFactor, decimFactor)), 0.0 }
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
    auto p = 0u;
    auto c = 0u;
    auto failed = 0u;
    for (auto i = 0u; i < ProcessRate - 1; ++i) {
        interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor, ProcessRate - 1 - i);
        auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
        p += produced;
        while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
            // std::cout << "consumed: " << std::endl;
            *output = decimData[0];
            ++output;
            c += decimFactor;
        }
    }
    // return;
    for (auto i = ProcessRate - 1; i < inputSize; ++i) {
        const std::size_t idx = i * ProcessRate;
        // std::cout << "ii::: " << idx << std::endl;
        // Produce interpFactor of samples
        interpolate<ProcessRate>(interpolateData.data(), input + i, filterCoefs.data(), interpFactor);
        auto produced = producedCache.pushRange(interpolateData.begin(), interpolateData.end());
        p += produced;
        // if (produced != interpFactor)
            // std::cout << "failed production: " << produced << "/" << filterSize << std::endl;
        // else
            // std::cout << "produced: " << produced << std::endl;
        // If possible, consume decimFactor samples
        while (producedCache.tryPopRange(decimData.begin(), decimData.end())) {
            // std::cout << "consumed: " << std::endl;
            *output = decimData[0];
            ++output;
            c += decimFactor;
        }
        // break;
    }
    // std::cout << "produced total: " << p << std::endl;
    // std::cout << "consumed total: " << c << std::endl;
    // std::cout << "::" << output << std::endl;
    // std::cout << "::" << output - out << std::endl;
}

template<typename Type>
inline Audio::DSP::FIR::VoidType<Type>
    Audio::DSP::FIR::Filter(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const bool zeroPad) noexcept
{
    std::cout << "FILTERING FIR..." << std::endl;

    const auto windowSize = specs.windowSize;
    std::vector<Type> windowFilter(windowSize);
    WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
    DesignFilter(specs, windowFilter.data(), windowSize);

    std::vector<Type> data(inputSize, 0.0);
    std::memcpy(data.data(), input, sizeof(Type) * inputSize);

    for (auto i = 0; i < inputSize; ++i) {
        if (i < windowSize - 1) {
            output[i] = FilterImpl(data.data() + i, windowFilter.data(), windowSize, windowSize - i - 1);
        } else {
            output[i] = FilterImpl(data.data() + i, windowFilter.data(), windowSize, 0u);
        }
    }
    std::for_each(output, output + inputSize, [](Type &value){
        value /= std::sqrt(2.0) * 2;
    });
    std::cout << "FILTERING FIR done." << std::endl;
}

template<typename Type>
inline Audio::DSP::FIR::VoidType<Type>
    Audio::DSP::FIR::Filter(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const Type *lastInput, const std::size_t lastInputSize) noexcept
{
    std::cout << "FILTERING FIR..." << std::endl;

    const auto windowSize = specs.windowSize;
    std::vector<Type> windowFilter(windowSize);
    WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
    DesignFilter(specs, windowFilter.data(), windowSize);

    std::vector<Type> data(inputSize + lastInputSize, 0.0);
    std::memcpy(data.data(), lastInput, sizeof(Type) * lastInputSize);
    std::memcpy(data.data() + lastInputSize, input, sizeof(Type) * inputSize);

    for (auto i = 0; i < inputSize; ++i) {
        const auto idx = i + lastInputSize;
        if (lastInputSize < windowSize && idx < windowSize - 1) {
            output[i] = FilterImpl(data.data() + idx, windowFilter.data(), windowSize, windowSize - idx - 1);
        } else {
            output[i] = FilterImpl(data.data() + idx, windowFilter.data(), windowSize, 0u);
        }
    }
}

template<typename Type>
inline Audio::DSP::FIR::ProcessType<Type>
    Audio::DSP::FIR::FilterImpl(const Type *input, const Type *window, const std::size_t size, const std::size_t zeroPadSize) noexcept
{
    Type sample { 0.0 };

    for (auto i = zeroPadSize; i < size; ++i) {
        const auto idx = size - i - 1;
        sample += (input[idx] * window[idx]);
        // sample += (input[i] * window[i]);
        // std::cout << input[i] << std::endl;
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
        *windowCoefficients *= (realRate * Utils::sinc(idx * realRate));
        ++windowCoefficients;
    }
}