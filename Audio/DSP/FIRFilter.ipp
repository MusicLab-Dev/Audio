/**
 * @file FIRFilter.ipp
 * @brief FIR Filters implementation
 *
 * @author Pierre V
 * @date 2021-04-24
 */

/**
 * @brief BasicFilter implementation
 */
template<typename Type>
inline void Audio::DSP::FIR::BasicFilter<Type>::init(const DSP::Filter::FIRSpec &specs) noexcept
{
    setSpecs(specs);
    resizeLastInputCache(_specs.size - 1u);
    resetLastInputCache();
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setSpecs(const DSP::Filter::FIRSpec &specs) noexcept
{
    if (_specs == specs)
        return false;
    _specs = specs;
    _instance.coefficients().resize(specs.size);
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
inline bool Audio::DSP::FIR::BasicFilter<Type>::setSize(const std::uint32_t size) noexcept
{
    resizeLastInputCache(size - 1ul);
    resetLastInputCache();
    return setSpecs(
        Filter::FIRSpec {
            _specs.filterType,
            _specs.windowType,
            size,
            _specs.sampleRate,
            { _specs.cutoffs[0], _specs.cutoffs[1] },
            _specs.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setFilterType(const DSP::Filter::BasicType filterType) noexcept
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
inline bool Audio::DSP::FIR::BasicFilter<Type>::setWindowType(const DSP::Filter::WindowType windowType) noexcept
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

/**
 * @brief MultiFilter implementation
 */
template<unsigned InstanceCount, typename Type>
template<typename GainType, std::uint32_t GainSize>
typename Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainType(&gains)[GainSize]) noexcept
{
    static_assert(GainSize <= InstanceCount, "Audio::DSP::FIR::MultiFilter::filter: gains input must not be greater than the internal InstanceCount.");
    for (auto i = 0u; i < GainSize; ++i) {
        _gains[i] = gains[i];
    }
    for (auto i = GainSize; i < InstanceCount; ++i) {
        _gains[i] = 0.0;
    }
    _instances.filter(input, inputSize, output, _gains);
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::init(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate, const double rootFreq) noexcept
{
    _windowType = windowType;
    _filterSize = filterSize;
    _sampleRate = sampleRate;
    // for (auto &cutoff : _cutoffs) {

    // }
    UNUSED(rootFreq);
    reloadInstances(_windowType, _filterSize, _sampleRate);
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::reloadInstance(const DSP::Filter::FIRSpec &specs, const std::uint32_t instanceIndex) noexcept
{
    _instances.coefficients()[instanceIndex].resize(specs.size);
    _instances.lastInput().resize(specs.size - 1);
    _instances.lastInput().clear();

    // Filter::GenerateFilter(specs, _instances.coefficients()[instanceIndex].data());
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::reloadInstances(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate) noexcept
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
    // Middle filters (band-pass)
    for (auto i = 1u; i < InstanceCount - 1ul; ++i) {
        reloadInstance(
            Filter::FIRSpec {
                Filter::BasicType::BandPass,
                windowType,
                filterSize,
                sampleRate,
                { _cutoffs[i - 1ul], _cutoffs[i]},
               _gains[i]
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
            { _cutoffs[InstanceCount - 2ul], 0.0 },
            _gains[InstanceCount - 1ul]
        },
        InstanceCount - 1ul
    );
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setSampleRate(const float sampleRate) noexcept
{
    if (_sampleRate == sampleRate)
        return false;
    _sampleRate = sampleRate;
    reloadInstances(_windowType, _filterSize, _sampleRate);
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setFilterSize(const std::uint32_t filterSize) noexcept
{
    if (_filterSize == filterSize)
        return false;
    _filterSize = filterSize;
    reloadInstances(_windowType, _filterSize, _filterSize);
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setWindowType(const DSP::Filter::WindowType windowType) noexcept
{
    if (_windowType == _windowType)
        return false;
    _windowType = _windowType;
    reloadInstances(_windowType, _filterSize, _windowType);
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setCutoffs(const Internal::CutoffList &cutoffs) noexcept
{
    for (const auto &x : cutoffs)
        std::cout << x << std::endl;
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setGains(const Internal::GainList &gains) noexcept
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
