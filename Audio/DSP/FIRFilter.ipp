/**
 * @file FIRFilter.ipp
 * @brief FIR Filters implementation
 *
 * @author Pierre V
 * @date 2021-04-24
 */

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setSpecs(const Filter::FIRSpec specs) noexcept
{
    if (_specs == specs)
        return false;
    _specs = specs;
    _instance.coefficients().resize(specs.size);
    _instance.lastInput().resize(specs.size - 1);
    _instance.lastInput().clear();
    Filter::GenerateFilter(specs, _instance.coefficients().data());

    return true;
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setCutoffs(const float cutoffFrom, const float cutoffTo) noexcept
{
    return setSpecs(
        Filter::FIRSpec {
            _specs.filterType,
            _specs.windowType,
            _specs.size,
            _specs.sampleRate,
            { cutoffFrom, cutoffTo },
            _specs.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setSampleRate(const float sampleRate) noexcept
{
    return setSpecs(
        Filter::FIRSpec {
            _specs.filterType,
            _specs.windowType,
            _specs.size,
            sampleRate,
            { _specs.cutoffs[0], _specs.cutoffs[1] },
            _specs.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setFilterType(const Filter::BasicType filterType) noexcept
{
    return setSpecs(
        Filter::FIRSpec {
            filterType,
            _specs.windowType,
            _specs.size,
            _specs.sampleRate,
            { _specs.cutoffs[0], _specs.cutoffs[1] },
            _specs.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setWindowType(const Filter::WindowType windowType) noexcept
{
    return setSpecs(
        Filter::FIRSpec {
            _specs.filterType,
            windowType,
            _specs.size,
            _specs.sampleRate,
            { _specs.cutoffs[0], _specs.cutoffs[1] },
            _specs.gain
        }
    );
}

#include <iostream>

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::reloadInstance(const Filter::FIRSpec &specs, const std::size_t instanceIndex) noexcept
{
    _instances[instanceIndex].coefficients().resize(specs.size);
    _instances[instanceIndex].lastInput().resize(specs.size - 1);
    _instances[instanceIndex].lastInput().clear();

    Filter::GenerateFilter(specs, _instances[instanceIndex].coefficients().data());
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::reloadInstances(const DSP::Filter::WindowType &windowType, const std::size_t filterSize, const float sampleRate) noexcept
{
    // First filter (low-pass)
    reloadInstance(
        Filter::FIRSpec {
            Filter::BasicType::LowPass,
            windowType,
            filterSize,
            sampleRate,
            { _cutoffs[0ul], 0.0 },
            _gains[0ul]
        },
        0ul
    );
    // Middle filters
    for (auto i = 1ul; i < InstanceCount - 1ul; ++i) {
        reloadInstance(
            Filter::FIRSpec {
                Filter::BasicType::BandPass,
                windowType,
                filterSize,
                sampleRate,
                { _cutoffs[i - 1ul], _cutoffs[i]},
               _gains[i - 1ul]
             },
            i
        );
    }
    // Last filter (low-pass)
    reloadInstance(
        Filter::FIRSpec {
            Filter::BasicType::HighPass,
            windowType,
            filterSize,
            sampleRate,
            { _cutoffs[InstanceCount - 1ul], 0.0 },
            _gains[InstanceCount - 1ul]
        },
        InstanceCount - 1ul
    );
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setSampleRate(const float sampleRate) noexcept
{
    if (_sampleRate == sampleRate)
        return false;
    _sampleRate = sampleRate;
    reloadInstances(_windowType, _filterSize, _sampleRate);
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setFilterSize(const std::size_t filterSize) noexcept
{
    if (_filterSize == filterSize)
        return false;
    _filterSize = filterSize;
    reloadInstances(_windowType, _filterSize, _filterSize);
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setWindowType(const Filter::WindowType windowType) noexcept
{
    if (_windowType == _windowType)
        return false;
    _windowType = _windowType;
    reloadInstances(_windowType, _filterSize, _windowType);
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setCutoffs(const CutoffList &cutoffs) noexcept
{
    for (const auto &x : cutoffs)
        std::cout << x << std::endl;
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setGains(const GainList &gains) noexcept
{
    for (const auto &x : gains)
        std::cout << x << std::endl;
    return true;
}

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setSampleRate(const float sampleRate) noexcept
// {
//     bool changed = false;

//     for (auto &instance : _instances) {
//         instance.s
//     }
// }
