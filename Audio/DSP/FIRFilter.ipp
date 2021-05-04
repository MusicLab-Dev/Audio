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
inline void Audio::DSP::FIR::BasicFilter<Type>::init(const DSP::Filter::FIRSpec &spec) noexcept
{
    setSpec(spec);
    resizeLastInputCache(_spec.order);
    resetLastInputCache();
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setSpec(const DSP::Filter::FIRSpec &spec) noexcept
{
    if (_spec == spec)
        return false;
    _spec = spec;
    _instance.coefficients().resize(spec.order + 1u);
    std::cout << "SET SPECS: " << spec.cutoffs[0] << " -> " << spec.cutoffs[1] << std::endl;
    Filter::GenerateFilter<true>(spec, _instance.coefficients().data());
    return true;
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setCutoffs(const float cutoffFrom, const float cutoffTo) noexcept
{
    return setSpec(
        Filter::FIRSpec {
            _spec.filterType,
            _spec.windowType,
            _spec.order,
            _spec.sampleRate,
            { cutoffFrom, cutoffTo },
            _spec.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::_setGain(const DB gain) noexcept
{
    return setSpec(
        Filter::FIRSpec {
            _spec.filterType,
            _spec.windowType,
            _spec.order,
            _spec.sampleRate,
            { _spec.cutoffs[0], _spec.cutoffs[1] },
            gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setSampleRate(const float sampleRate) noexcept
{
    return setSpec(
        Filter::FIRSpec {
            _spec.filterType,
            _spec.windowType,
            _spec.order,
            sampleRate,
            { _spec.cutoffs[0], _spec.cutoffs[1] },
            _spec.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setOrder(const std::uint32_t order) noexcept
{
    resizeLastInputCache(order);
    resetLastInputCache();
    return setSpec(
        Filter::FIRSpec {
            _spec.filterType,
            _spec.windowType,
            order,
            _spec.sampleRate,
            { _spec.cutoffs[0], _spec.cutoffs[1] },
            _spec.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setFilterType(const DSP::Filter::BasicType filterType) noexcept
{
    return setSpec(
        Filter::FIRSpec {
            filterType,
            _spec.windowType,
            _spec.order,
            _spec.sampleRate,
            { _spec.cutoffs[0], _spec.cutoffs[1] },
            _spec.gain
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIR::BasicFilter<Type>::setWindowType(const DSP::Filter::WindowType windowType) noexcept
{
    return setSpec(
        Filter::FIRSpec {
            _spec.filterType,
            windowType,
            _spec.order,
            _spec.sampleRate,
            { _spec.cutoffs[0], _spec.cutoffs[1] },
            _spec.gain
        }
    );
}


/**
 * @brief BandFilter implementation
 */
template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::BandFilter<InstanceCount, Type>::init(const Audio::DSP::Filter::WindowType windowType, const float sampleRate, const std::uint32_t order) noexcept
{
    const auto filterSize = order + 1u;
    _instance.coefficients().resize(filterSize);
    _filterOrder = order;
    _windowType = windowType;
    _sampleRate = sampleRate;
    _gain.resize(InstanceCount);
    for (auto &coef: _coefficients)
        coef.resize(filterSize);
    resizeLastInputCache(order);
    resetLastInputCache();
    reloadAll();
}

template<unsigned InstanceCount, typename Type>
template<bool Accumulate, typename GainType, std::uint32_t GainSize>
typename Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::BandFilter<InstanceCount, Type>::filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainType(&gains)[GainSize]) noexcept
{
    // std::cout << "FIlter:::" << std::endl;
    // ASSERT GainSize == InstanceCount
    auto ProcessGainUpdate = [&](const std::uint32_t filterIndex, const auto gain, const auto newGain) -> bool
    {
        if (gain == newGain)
            return false;
        std::cout << "  update(" << filterIndex << "): " << gain << " -> " << newGain << std::endl;
        _gain[filterIndex] = newGain;
        switch (filterIndex) {
        case 0u:
            reloadLowPass(RootFrequency, newGain);
            break;
        case InstanceCount - 1:
            reloadHighPass(static_cast<std::uint32_t>(RootFrequency) << (InstanceCount == MinBandFilterSize ? 0u : (filterIndex - 1u)), newGain);
            break;
        default:
            reloadBandPass(filterIndex, static_cast<std::uint32_t>(RootFrequency) << (filterIndex - 1u), newGain);
            break;
        }
        return true;
    };

    bool updated { false };
    for (auto i = 0u; i < GainSize; ++i) {
        if (ProcessGainUpdate(i, _gain[i], gains[i]) && !updated)
            updated = true;
    }
    if (updated) {
        mergeToInstance();
    }
    UNUSED(input);
    UNUSED(inputSize);
    UNUSED(output);
    _instance.template filter<Accumulate>(input, inputSize, output, 1.0f);
    // for (auto i = 0u; i < InstanceCount; ++i) {
    //     _instance.template filter<Accumulate>(input, inputSize, output, 1.0f);
    // }
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::BandFilter<InstanceCount, Type>::mergeToInstance(void) noexcept
{
    for (auto k = 0u; k <= _filterOrder; ++k) {
        _instance.coefficients()[k] = _coefficients[0][k];
    }
    for (auto i = 1u; i < InstanceCount; ++i) {
        for (auto k = 0u; k <= _filterOrder; ++k) {
            _instance.coefficients()[k] += _coefficients[i][k];
        }
    }
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::BandFilter<InstanceCount, Type>::reloadAll(void) noexcept
{
    auto rootFreq = RootFrequency;
    // Take the centered frequency
    if constexpr (InstanceCount != MinBandFilterSize)
        rootFreq = rootFreq * 3 / 2;
    // Low-pass filters
    reloadLowPass(rootFreq, 1.0f);
    // Band-pass filters
    for (auto i = 1u; i < InstanceCount - 1u; ++i) {
        reloadBandPass(i, rootFreq, 1.0f);
        rootFreq *= 2.f;
    }
    if constexpr (InstanceCount != MinBandFilterSize)
        rootFreq *= 2.f;
    // High-pass filters
    reloadHighPass(rootFreq, 1.0f);
    mergeToInstance();
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::BandFilter<InstanceCount, Type>::reloadLowPass(const float rootFreq, const float gain) noexcept
{
    std::cout << "Reload low pass " << rootFreq << ", " << gain << std::endl;
    Filter::GenerateFilter<true, false>(
        Filter::FIRSpec {
            Filter::BasicType::LowPass,
            Filter::WindowType::Default,
            _filterOrder,
            _sampleRate,
            { rootFreq, 0.0f },
            gain
        },
        _coefficients[0].data()
    );
}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::BandFilter<InstanceCount, Type>::reloadBandPass(const std::uint32_t filterIndex, const float rootFreq, const float gain) noexcept
{
    std::cout << "Reload band pass " << rootFreq << ", " << gain << std::endl;
    Filter::GenerateFilter<true, false>(
        Filter::FIRSpec {
            Filter::BasicType::BandPass,
            Filter::WindowType::Default,
            _filterOrder,
            _sampleRate,
            { rootFreq, rootFreq * 2.f },
            gain
        },
        _coefficients[filterIndex].data()
    );

}

template<unsigned InstanceCount, typename Type>
inline void Audio::DSP::FIR::BandFilter<InstanceCount, Type>::reloadHighPass(const float rootFreq, const float gain) noexcept
{
    std::cout << "Reload high pass " << rootFreq << ", " << gain << std::endl;
    Filter::GenerateFilter<true, false>(
        Filter::FIRSpec {
            Filter::BasicType::HighPass,
            Filter::WindowType::Default,
            _filterOrder,
            _sampleRate,
            { rootFreq, 0.0f },
            gain
        },
        _coefficients[InstanceCount - 1u].data()
    );
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::BandFilter<InstanceCount, Type>::setSampleRate(const float sampleRate) noexcept
{
    if (_sampleRate == sampleRate)
        return false;
    _sampleRate = sampleRate;
    // reload window
    return true;
}

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::BandFilter<InstanceCount, Type>::setOrder(const std::uint32_t order) noexcept
{
    resizeLastInputCache(order);
    resetLastInputCache();
    if (_filterOrder == order)
        return false;
    _filterOrder = order;
    // reload window
    return true;
}

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::BandFilter<InstanceCount, Type>::setFilterType(const std::size_t filterIndex, const DSP::Filter::BasicType filterType) noexcept
// {
//     return setSpec(filterIndex, filterType, _gain[filterIndex], _cutoffs[filterIndex], _cutoffs[filterIndex * 2]);
// }

template<unsigned InstanceCount, typename Type>
inline bool Audio::DSP::FIR::BandFilter<InstanceCount, Type>::setWindowType(const DSP::Filter::WindowType windowType) noexcept
{
    if (_windowType == windowType)
        return false;
    _windowType = windowType;
    // reload window
    return true;
}

/**
 * @brief SerieFilter implementation
 */
// template<unsigned InstanceCount, typename Type>
// template<typename GainType, std::uint32_t GainSize>
// typename Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::filter(const Type *input, const std::uint32_t inputSize, Type *output, const GainType(&gains)[GainSize]) noexcept
// {
//     static_assert(GainSize <= InstanceCount, "Audio::DSP::FIR::SerieFilter::filter: gains input must not be greater than the internal InstanceCount.");
//     for (auto i = 0u; i < GainSize; ++i) {
//         _gains[i] = gains[i];
//     }
//     for (auto i = GainSize; i < InstanceCount; ++i) {
//         _gains[i] = 0.0;
//     }
//     _instances.filter(input, inputSize, output, _gains);
// }

// template<unsigned InstanceCount, typename Type>
// inline void Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::init(const DSP::Filter::WindowType windowType, const std::uint32_t filterSize, const float sampleRate, const double rootFreq) noexcept
// {
//     _windowType = windowType;
//     _filterOrder = filterOrder;
//     _sampleRate = sampleRate;
//     // for (auto &cutoff : _cutoffs) {

//     // }
//     UNUSED(rootFreq);
//     reloadInstances(_windowType, _filterOrder, _sampleRate);
// }

// template<unsigned InstanceCount, typename Type>
// inline void Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::reloadInstance(const DSP::Filter::FIRSpec &spec, const std::uint32_t instanceIndex) noexcept
// {
//     _instances.coefficients()[instanceIndex].resize(spec.order + 1u);
//     _instances.lastInput().resize(spec.order);
//     _instances.lastInput().clear();

//     // Filter::GenerateFilter(spec, _instances.coefficients()[instanceIndex].data());
// }

// template<unsigned InstanceCount, typename Type>
// inline void Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::reloadInstances(const DSP::Filter::WindowType windowType, const std::uint32_t filterOrder, const float sampleRate) noexcept
// {
//     // First filter (low-pass)
//     reloadInstance(
//         Filter::FIRSpec {
//             Filter::BasicType::LowPass,
//             windowType,
//             filterOrder,
//             sampleRate,
//             { _cutoffs[0ul], 0.0 },
//             _gains[0ul]
//         },
//         0ul
//     );
//     // Middle filters (band-pass)
//     for (auto i = 1u; i < InstanceCount - 1ul; ++i) {
//         reloadInstance(
//             Filter::FIRSpec {
//                 Filter::BasicType::BandPass,
//                 windowType,
//                 filterOrder,
//                 sampleRate,
//                 { _cutoffs[i - 1ul], _cutoffs[i]},
//                _gains[i]
//              },
//             i
//         );
//     }
//     // Last filter (low-pass)
//     reloadInstance(
//         Filter::FIRSpec {
//             Filter::BasicType::HighPass,
//             windowType,
//             filterOrder,
//             sampleRate,
//             { _cutoffs[InstanceCount - 2ul], 0.0 },
//             _gains[InstanceCount - 1ul]
//         },
//         InstanceCount - 1ul
//     );
// }

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setSampleRate(const float sampleRate) noexcept
// {
//     if (_sampleRate == sampleRate)
//         return false;
//     _sampleRate = sampleRate;
//     reloadInstances(_windowType, _filterOrder, _sampleRate);
//     return true;
// }

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setFilterSize(const std::uint32_t filterSize) noexcept
// {
//     if (_filterOrder == filterOrder)
//         return false;
//     _filterOrder = filterOrder;
//     reloadInstances(_windowType, _filterOrder, _sampleRate);
//     return true;
// }

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setWindowType(const DSP::Filter::WindowType windowType) noexcept
// {
//     if (_windowType == _windowType)
//         return false;
//     _windowType = _windowType;
//     reloadInstances(_windowType, _filter, _sampleRate);
//     return true;
// }

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setCutoffs(const Internal::CutoffList &cutoffs) noexcept
// {
//     for (const auto &x : cutoffs)
//         std::cout << x << std::endl;
//     return true;
// }

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::SerieFilter<InstanceCount, Type>::setGains(const Internal::GainList &gains) noexcept
// {
//     for (const auto &x : gains)
//         std::cout << x << std::endl;
//     return true;
// }

// template<unsigned InstanceCount, typename Type>
// inline bool Audio::DSP::FIR::MultiFilter<InstanceCount, Type>::setSampleRate(const float sampleRate) noexcept
// {
//     bool changed = false;

//     for (auto &instance : _instances) {
//         instance.s
//     }
// }
