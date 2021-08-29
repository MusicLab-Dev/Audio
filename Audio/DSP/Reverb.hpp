/**
 * @file Delay.hpp
 * @brief Reverb instances
 *
 * @author Pierre V
 * @date 2021-08-28
 */

#pragma once

#include "Delay.hpp"
#include "FIR.hpp"

#define GetSchroederImpl(Token) Token ## 1

static constexpr auto DefaultFeedback = 0.6f;

static constexpr auto SchroederSampleRateRef = 25000.0f;
static constexpr auto SchroederComb_00 = 901u;
static constexpr auto SchroederComb_10 = 778u;
static constexpr auto SchroederComb_20 = 1011u;
static constexpr auto SchroederComb_30 = 1123u;
static constexpr auto SchroederAP_00 = 125u;
static constexpr auto SchroederAP_10 = 42u;
static constexpr auto SchroederAP_20 = 12u;

static constexpr auto SchroederAP_01 = 1051u;
static constexpr auto SchroederAP_11 = 337u;
static constexpr auto SchroederAP_21 = 113u;
static constexpr auto SchroederComb_01 = 4799u;
static constexpr auto SchroederComb_11 = 4999u;
static constexpr auto SchroederComb_21 = 5399u;
static constexpr auto SchroederComb_31 = 5801u;


static constexpr auto GetSchroederDelayTime = [](const auto size)
{
    return static_cast<float>(size) / SchroederSampleRateRef;
};


namespace Audio::DSP::Reverb
{
    namespace Internal
    {
        static constexpr auto DefaultAllPassGain = 0.5f;

        static constexpr auto BasicCombCount = 4u;
        static constexpr auto BasicAllPassCount = 3u;

        static constexpr float BasicCombTime[BasicCombCount]
        {
            17.0f / 1000.0f,
            29.0f / 1000.0f,
            37.0f / 1000.0f,
            47.0f / 1000.0f
        };
        static constexpr float BasicAllPassTime[BasicAllPassCount]
        {
            12.6077097506f / 1000.0f,
            10.0f / 1000.0f,
            7.73242630385f / 1000.0f
        };

        template<typename Type>
        using BasicComb = DelayLineParallel<Type, DelayLineDesign::Feedback, BasicCombCount>;

        template<typename Type>
        using BasicAllPass = DelayLineSerie<Type, DelayLineDesign::AllPass, BasicAllPassCount>;

    }

    /** @brief A parallel comb filters into a serie of all-pass filters */
    template<typename Type>
    class Basic;
}

template<typename Type>
class Audio::DSP::Reverb::Basic
{
public:
    void reset(const AudioSpecs &audioSpecs) noexcept;
    void setSampleRate(const SampleRate sampleRate) noexcept { _sampleRate = sampleRate; }
    void setPreDelayDuration(const float preDelayDuration) noexcept;
    void setReverbDuration(const float reverbDuration) noexcept;


    void process(const Type *input, Type *output, const std::size_t processSize, const DB outGain = 1.0f) noexcept;


private:
    SampleRate _sampleRate;
    float _preDelayDuration;
    float _reverbDuration;

    FIR::BasicFilter<Type> _bandFilter;
    DelayLineUnique<Type, DelayLineDesign::Default> _preDelay;
    Internal::BasicComb<Type> _combs;
    Internal::BasicAllPass<Type> _allPass;
};

#include "Reverb.ipp"
