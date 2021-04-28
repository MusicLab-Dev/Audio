/**
 * @file FIR.hpp
 * @brief FIR filtering
 *
 * @author Pierre V
 * @date 2021-04-22
 */

#pragma once

#include <Core/Vector.hpp>

#include "Filter.hpp"

namespace Audio::DSP::FIR
{
    namespace Internal
    {
        template<typename Type>
        using Cache = Core::TinyVector<Type>;
        template<unsigned InstanceCount, typename Type>
        using CacheList = std::array<Cache<Type>, InstanceCount>;

        using CutoffList = std::initializer_list<float>;
        using GainList = std::initializer_list<DB>;
        template<unsigned InstanceCount>
        using GainArray = std::array<DB, InstanceCount>;

        /** @brief Instance with coefficients cache & lastInputCache */
        template<typename Type>
        class Instance;
        /** @brief Instance with multiple coefficients caches */
        template<unsigned InstanceCount, typename Type>
        class MultiInstance;
    }

    /** @brief Filter using a single FIR instance */
    template<typename Type>
    class BasicFilter;

    /** @brief Filter using a single FIR instance */
    template<unsigned InstanceCount, typename Type>
    class BandFilter;

    /** @brief Filter using multiple FIR instances */
    template<unsigned InstanceCount, typename Type>
    class SerieFilter;


    template<typename Type>
    using VoidType = std::enable_if_t<std::is_floating_point_v<Type>, void>;
    template<typename Type>
    using ProcessType = std::enable_if_t<std::is_floating_point_v<Type>, Type>;
}

template<typename Type>
class Audio::DSP::FIR::Internal::Instance
{
public:
    /** @brief Perform filtering using convolution. */
    template<bool Accumulate>
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const Type outGain) noexcept;

    /** @brief Get the internal cache coefficients */
    [[nodiscard]] const Cache<Type> &coefficients(void) const noexcept { return _coefficients; }
    [[nodiscard]] Cache<Type> &coefficients(void) noexcept { return _coefficients; }

    /** @brief Get the internal cache for the last input */
    [[nodiscard]] const Cache<Type> &lastInput(void) const noexcept { return _lastInputCache; }
    [[nodiscard]] Cache<Type> &lastInput(void) noexcept { return _lastInputCache; }

private:
    template<bool Accumulate>
    ProcessType<Type> filterImpl(const Type *input, const std::uint32_t size, const std::uint32_t zeroPad = 0ul) noexcept;

    /** @brief Filter cache coefficients */
    Cache<Type> _coefficients;
    /** @brief Last input cache used for block processing */
    Cache<Type> _lastInputCache;
};

template<unsigned InstanceCount, typename Type>
class Audio::DSP::FIR::Internal::MultiInstance
{
public:
    /** @brief Perform filtering using convolution and gains for each instance */
    template<bool Accumulate>
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainArray<InstanceCount> &gains) noexcept;

    /** @brief Get the internal cache coefficients */
    [[nodiscard]] const CacheList<InstanceCount, Type> &coefficients(void) const noexcept { return _coefficients; }
    [[nodiscard]] CacheList<InstanceCount, Type> &coefficients(void) noexcept { return _coefficients; }

    /** @brief Get the internal cache for the last input */
    [[nodiscard]] const Cache<Type> &lastInput(void) const noexcept { return _lastInputCache; }
    [[nodiscard]] Cache<Type> &lastInput(void) noexcept { return _lastInputCache; }

private:
    template<bool Accumulate>
    ProcessType<Type> filterImpl(const Type *input, const std::uint32_t size, const Type *coef, const std::uint32_t zeroPad = 0ul) noexcept;

    /** @brief Filter cache coefficients */
    Internal::CacheList<InstanceCount, Type> _coefficients;
    /** @brief Last input cache used for block processing */
    Cache<Type> _lastInputCache;
};


template<typename Type>
class Audio::DSP::FIR::BasicFilter
{
public:
    BasicFilter(void) = default;
    BasicFilter(const DSP::Filter::FIRSpec &spec) { init(spec); }

    /** @brief Initialize the internal filter spec */
    void init(const DSP::Filter::FIRSpec &spec) noexcept;

    /** @brief Set the internal spec. It will recompute the instance coefficients */
    bool setSpec(const DSP::Filter::FIRSpec &spec) noexcept;
    /** @brief Set the internal cutoffs */
    bool setCutoffs(const float cutoffFrom, const float cutoffTo = 0.0f) noexcept;
    /** @brief Set the internal sampleRate */
    bool setSampleRate(const float sampleRate) noexcept;
    /** @brief Set the internal sampleRate */
    bool setSize(const std::uint32_t ize) noexcept;
    /** @brief Set the internal filter type */
    bool setFilterType(const DSP::Filter::BasicType filterType) noexcept;
    /** @brief Set the internal window type */
    bool setWindowType(const DSP::Filter::WindowType windowType) noexcept;

    /** @brief Reset the internal last input cache */
    void resetLastInputCache(void) noexcept { _instance.lastInput().clear(); }
    /** @brief Resize the internal last input cache */
    void resizeLastInputCache(const std::uint32_t size) noexcept { _instance.lastInput().resize(size); }

    /** @brief Call the filter instance */
    template<bool Accumulate = false>
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const Type outGain = 1.0) noexcept { _instance.template filter<Accumulate>(input, inputSize, output, outGain); }

private:
    /** @brief Internal instance */
    Internal::Instance<Type> _instance;
    /** @brief Filter spec */
    DSP::Filter::FIRSpec _spec;
};

template<unsigned InstanceCount, typename Type>
class Audio::DSP::FIR::BandFilter
{
    static constexpr auto MinBandFilterSize = 2u;
    static constexpr auto TenBandFilterSize = 10u;

    /** @brief Construct only for 2 or more instances */
    static_assert(InstanceCount >= MinBandFilterSize, "Audio::DSP::FIR::MultiFilter need at least 2 instances");
    // static_assert((InstanceCount == MinBandFilterSize) || (InstanceCount == SmallBandFilterSize), "Audio::DSP::FIR::MultiFilter only support 10 instances");

    static constexpr float MinBandFilterRootFrequency = 8'000.0f;
    static constexpr float TenBandFilterRootFrequency = 32.0f;

public:
    BandFilter(void) = default;
    BandFilter(const DSP::Filter::WindowType windowType, const float sampleRate, const std::uint32_t size) { init(windowType, sampleRate, size); }

    /** @brief Initialize the internal filter specs */
    void init(const DSP::Filter::WindowType windowType, const float sampleRate, const std::uint32_t size) noexcept;

    /** @brief Set the internal window type */
    bool setWindowType(const DSP::Filter::WindowType windowType) noexcept;
    /** @brief Set the internal sampleRate */
    bool setSize(const std::uint32_t ize) noexcept;
    /** @brief Set the internal sampleRate */
    bool setSampleRate(const float sampleRate) noexcept;

    /** @brief Reset the internal last input cache */
    void resetLastInputCache(void) noexcept { _instance.lastInput().clear(); }
    /** @brief Resize the internal last input cache */
    void resizeLastInputCache(const std::uint32_t size) noexcept { _instance.lastInput().resize(size); }

    /** @brief Call the filter instance */
    template<bool Accumutale = false, typename GainType, std::uint32_t GainSize>
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainType(&gains)[GainSize]) noexcept;

private:
    /** @brief Internal instance */
    Internal::Instance<Type> _instance;
    /** @brief Filter specs */
    DSP::Filter::WindowType _windowType { DSP::Filter::WindowType::Hanning };
    float _sampleRate;
    std::uint32_t _filterSize;
    Core::TinyVector<float> _gain;
    Internal::CacheList<InstanceCount, Type> _coefficients;

    void reloadAll(void) noexcept;
    void mergeToInstance(void) noexcept;
    void reloadLowPass(const float rootFreq, const float gain) noexcept;
    void reloadBandPass(const std::uint32_t filterIndex, const float rootFreq, const float gain) noexcept;
    void reloadHighPass(const float rootFreq, const float gain) noexcept;
};

template<unsigned InstanceCount, typename Type>
class Audio::DSP::FIR::SerieFilter
{
public:
    using CutoffArray = std::array<float, InstanceCount - 1ul>;

    SerieFilter(void) = default;
    SerieFilter(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate, const double rootFreq) { init(windowType, filterSize, sampleRate, rootFreq); }

    /** @brief Initialize the internal filters specs */
    void init(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate, const double rootFreq) noexcept;

    /** @brief Set the internal cutoffs */
    bool setCutoffs(const Internal::CutoffList &cutoffs) noexcept;
    /** @brief Set the internal cutoffs */
    bool setGains(const Internal::GainList &cutoffs) noexcept;
    /** @brief Set the internal sampleRate */
    bool setSampleRate(const float sampleRate) noexcept;
    /** @brief Set the internal window type */
    bool setWindowType(const DSP::Filter::WindowType windowType) noexcept;
    /** @brief Set the internal sampleRate */
    bool setFilterSize(const std::uint32_t filterSize) noexcept;

    /** @brief Reset all gains to 0dB */
    void resetGains(void) noexcept { _gains.fill(0.0); }

    // bool

    /** @brief Reset the internal last input cache */
    void resetLastInputCache(void) noexcept { _instances.lastInput().clear(); }

    /** @brief Call the filter instance */
    // VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output) noexcept { _instances.filter(input, inputSize, output); }

    /** @brief Call the filter instance with specific gains for each instance */
    template<typename GainType, std::uint32_t GainSize>
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainType(&gains)[GainSize]) noexcept;

private:
    /** @brief Internal instances */
    Internal::MultiInstance<InstanceCount, Type> _instances;

    /** @brief Filters specs */
    Filter::WindowType _windowType { Filter::WindowType::Hanning };
    std::uint32_t _filterSize;
    float _sampleRate;

    /** @brief Detached filters specs */
    CutoffArray _cutoffs;
    Internal::GainArray<InstanceCount> _gains;

    void reloadInstances(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate) noexcept;
    void reloadInstance(const DSP::Filter::FIRSpec &spec, std::uint32_t instanceIndex) noexcept;
};

#include "FIR.ipp"
#include "FIRFilter.ipp"
