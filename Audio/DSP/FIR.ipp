/**
 * @ Author: Pierre Veysseyre
 * @ Description: FIR.ipp
 */

#include <Audio/Math.hpp>

template<typename Type>
inline Audio::DSP::FIR::CallType<Type>
    Audio::DSP::FIR::Filter(const FilterSpecs specs, const Type *input, const std::size_t size, Type *output) noexcept
{
    std::cout << "FILTERING FIR..." << std::endl;

    const auto windowSize = specs.windowLenght;
    std::vector<Type> windowFilter(windowSize);
    WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
    DesignFilter(specs, windowFilter.data(), windowSize);

    std::vector<Type> data(windowSize + size - 1, 0.0);
    std::memcpy(data.data() + windowSize, input, sizeof(Type) * size);

    for (auto i = 0; i < size; ++i) {
        output[i] = FilterImpl(data.data() + i, windowFilter.data(), windowSize);
    }
    std::cout << "FILTERING FIR done." << std::endl;
}

template<typename Type>
inline Audio::DSP::FIR::ProcessType<Type>
    Audio::DSP::FIR::FilterImpl(const Type *input, const Type *window, const std::size_t size) noexcept
{
    Type sample { 0.0 };

    for (auto i = 0u; i < size; ++i) {
        sample += (input[i] * window[size - i - 1]);
        // sample += (input[i] * window[i]);
        // std::cout << input[i] << std::endl;
    }
    return sample / 2;
}


inline void Audio::DSP::FIR::DesignFilter(const FilterSpecs specs, float *windowCoefficients, const std::size_t windowSize, bool centered) noexcept
{
    const float cutoffRate = specs.cutoffs[0] / static_cast<float>(specs.sampleRate);
    switch (specs.filterType) {
    case FilterType::LowPass:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate, centered);

    default:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate, centered);
    }
}

inline void Audio::DSP::FIR::DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, bool centered) noexcept
{
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= (realRate * Utils::sinc(idx * realRate));
        ++windowCoefficients;
    }
}