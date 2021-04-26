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

    /** @brief Filter using multiple FIR instances */
    template<unsigned InstanceCount, typename Type>
    class MultiFilter;


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
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const Type outGain) noexcept;

    /** @brief Get the internal cache coefficients */
    [[nodiscard]] const Cache<Type> &coefficients(void) const noexcept { return _coefficients; }
    [[nodiscard]] Cache<Type> &coefficients(void) noexcept { return _coefficients; }

    /** @brief Get the internal cache for the last input */
    [[nodiscard]] const Cache<Type> &lastInput(void) const noexcept { return _lastInputCache; }
    [[nodiscard]] Cache<Type> &lastInput(void) noexcept { return _lastInputCache; }

private:
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
    using CacheList = std::array<Cache<Type>, InstanceCount>;

    /** @brief Perform filtering using convolution and gains for each instance */
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainArray<InstanceCount> &gains) noexcept;

    /** @brief Get the internal cache coefficients */
    [[nodiscard]] const CacheList &coefficients(void) const noexcept { return _coefficients; }
    [[nodiscard]] CacheList &coefficients(void) noexcept { return _coefficients; }

    /** @brief Get the internal cache for the last input */
    [[nodiscard]] const Cache<Type> &lastInput(void) const noexcept { return _lastInputCache; }
    [[nodiscard]] Cache<Type> &lastInput(void) noexcept { return _lastInputCache; }

private:
    ProcessType<Type> filterImpl(const Type *input, const std::uint32_t size, const Type *coef, const std::uint32_t zeroPad = 0ul) noexcept;

    /** @brief Filter cache coefficients */
    CacheList _coefficients;
    /** @brief Last input cache used for block processing */
    Cache<Type> _lastInputCache;
};


template<typename Type>
class Audio::DSP::FIR::BasicFilter
{
public:
    BasicFilter(void) = default;
    BasicFilter(const DSP::Filter::FIRSpec specs) { init(specs); }

    /** @brief Initialize the internal filter specs */
    void init(const DSP::Filter::FIRSpec &specs) noexcept;

    /** @brief Set the internal specs. It will recompute the instance coefficients */
    bool setSpecs(const DSP::Filter::FIRSpec &specs) noexcept;
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
    VoidType<Type> filter(const Type *input, const std::uint32_t inputSize, Type *output, const Type outGain = 1.0) noexcept { _instance.filter(input, inputSize, output, outGain); }

private:
    /** @brief Internal instance */
    Internal::Instance<Type> _instance;
    /** @brief Filter specs */
    DSP::Filter::FIRSpec _specs;
};

template<unsigned InstanceCount, typename Type>
class Audio::DSP::FIR::MultiFilter
{
    /** @brief Construct only for 2 or more instances */
    static_assert(InstanceCount > 1, "Audio::DSP::FIR::MultiFilter need at least 2 instances");

public:
    using CutoffArray = std::array<float, InstanceCount - 1ul>;

    MultiFilter(void) = default;
    MultiFilter(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate, const double rootFreq) { init(windowType, filterSize, sampleRate, rootFreq); }

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
    void reloadInstance(const DSP::Filter::FIRSpec &specs, std::uint32_t instanceIndex) noexcept;
};

#include "FIR.ipp"
#include "FIRFilter.ipp"
