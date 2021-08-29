/**
 * @file Delay.hpp
 * @brief Reverb instances implementation
 *
 * @author Pierre V
 * @date 2021-08-28
 */

template<typename Type>
inline void Audio::DSP::Reverb::Basic<Type>::reset(const AudioSpecs &audioSpecs) noexcept
{
    _sampleRate = audioSpecs.sampleRate;

    _bandFilter.init(
        DSP::Filter::FIRSpecs(
            DSP::Filter::BasicType::BandPass,
            DSP::Filter::WindowType::Hanning,
            33ul,
            static_cast<float>(_sampleRate),
            700.0f,
            7000.0f,
            1.0f
        )
    );

    _preDelay.setInputAmount(0.0f);
    _preDelay.setDelayAmount(0.0f);
    _preDelay.setDelayTime(static_cast<float>(_sampleRate), _preDelayDuration);
    _preDelay.reset(audioSpecs, 1.0f, _preDelayDuration);

    _combs.template reset<0u>(audioSpecs, 1.0f, Internal::BasicCombTime[0u]);
    _combs.template reset<1u>(audioSpecs, 1.0f, Internal::BasicCombTime[1u]);
    _combs.template reset<2u>(audioSpecs, 1.0f, Internal::BasicCombTime[2u]);
    _combs.template reset<3u>(audioSpecs, 1.0f, Internal::BasicCombTime[3u]);

    _allPass.template reset<0u>(audioSpecs, 1.0f, Internal::BasicAllPassTime[0u]);
    _allPass.template reset<1u>(audioSpecs, 1.0f, Internal::BasicAllPassTime[1u]);
    _allPass.template reset<2u>(audioSpecs, 1.0f, Internal::BasicAllPassTime[2u]);
    _allPass.template setDelayAmount<0u>(Internal::DefaultAllPassGain);
    _allPass.template setDelayAmount<1u>(Internal::DefaultAllPassGain);
    _allPass.template setDelayAmount<2u>(Internal::DefaultAllPassGain);
}

template<typename Type>
inline void Audio::DSP::Reverb::Basic<Type>::setReverbDuration(const float reverbDuration) noexcept
{
    static constexpr auto GetGain = [](const float reverbTime, const float delayTime)
    {
        return 1.0f / std::pow(10.0f, 3.0f * delayTime / reverbTime);
    };

    // if (reverbDuration == _reverbDuration)
    //     return;

    _reverbDuration = reverbDuration;

    // std::cout << reverbDuration << " :: " << Internal::BasicCombTime[0u] << " -> " << GetGain(_reverbDuration, Internal::BasicCombTime[0u]) << std::endl;

    _combs.template setDelayAmount<0u>(GetGain(_reverbDuration, Internal::BasicCombTime[0u]));
    _combs.template setDelayAmount<1u>(GetGain(_reverbDuration, Internal::BasicCombTime[1u]));
    _combs.template setDelayAmount<2u>(GetGain(_reverbDuration, Internal::BasicCombTime[2u]));
    _combs.template setDelayAmount<3u>(GetGain(_reverbDuration, Internal::BasicCombTime[3u]));
}

template<typename Type>
inline void Audio::DSP::Reverb::Basic<Type>::setPreDelayDuration(const float preDelayDuration) noexcept
{
    if (preDelayDuration == _preDelayDuration)
        return;

    _preDelayDuration = preDelayDuration;
    _preDelay.setDelayTime(static_cast<float>(_sampleRate), _preDelayDuration);
}

template<typename Type>
inline void Audio::DSP::Reverb::Basic<Type>::process(const Type *input, Type *output, const std::size_t processSize, const DB outGain) noexcept
{
    _bandFilter.filter(input, static_cast<std::uint32_t>(processSize), output);

    _preDelay.template process<0u, false>(input, output, processSize);
    _combs.template process<false>(output, output, processSize);
    _allPass.template process<false>(output, output, processSize);

    UNUSED(outGain);
}
