/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

#include <Audio/DSP/Merge.hpp>



// 0n: combs (feedback) -> allPass
// 1n: allPass -> combs (feedforward)

// Impl == (0 | 1)
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


inline void Audio::SimpleReverb::onAudioParametersChanged(void)
{
    _combFilters.resetAll(audioSpecs(), 10.0f, static_cast<float>(reverbTime()));
    _inputCache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
}

inline void Audio::SimpleReverb::onAudioGenerationStarted(const BeatRange &)
{
    _inputCache.clear();
    // _combFilters.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
}

inline void Audio::SimpleReverb::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    const auto outSize = output.size<float>();

    output.clear();
    if (static_cast<bool>(byBass())) {
        std::memcpy(out, _inputCache.data<float>(), output.size<std::uint8_t>());
        return;
    }




    _combFilters.setAmount<0u>(static_cast<float>(feedbackRate()), 1.0f);
    _combFilters.setAmount<1u>(static_cast<float>(feedbackRate()), 1.0f);
    _combFilters.setAmount<2u>(static_cast<float>(feedbackRate()), 1.0f);
    _combFilters.setAmount<3u>(static_cast<float>(feedbackRate()), 1.0f);
    _combFilters.setDelayTime<0u>(static_cast<float>(audioSpecs().sampleRate), GetSchroederDelayTime(SchroederComb_00) * static_cast<float>(reverbTime()));
    _combFilters.setDelayTime<1u>(static_cast<float>(audioSpecs().sampleRate), GetSchroederDelayTime(GetSchroederImpl(SchroederComb_1)) * static_cast<float>(reverbTime()));
    _combFilters.setDelayTime<2u>(static_cast<float>(audioSpecs().sampleRate), GetSchroederDelayTime(GetSchroederImpl(SchroederComb_2)) * static_cast<float>(reverbTime()));
    _combFilters.setDelayTime<3u>(static_cast<float>(audioSpecs().sampleRate), GetSchroederDelayTime(GetSchroederImpl(SchroederComb_3)) * static_cast<float>(reverbTime()));
    _combFilters.receiveDataAll<false>(out, outSize, static_cast<float>(mixRate()));

}

inline void Audio::SimpleReverb::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(byBass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _inputCache, inGain, true);
    if (!static_cast<bool>(byBass())) {
        _combFilters.setAmount<0u>(static_cast<float>(feedbackRate()), 1.0f);
        _combFilters.setAmount<1u>(static_cast<float>(feedbackRate()), 1.0f);
        _combFilters.setAmount<2u>(static_cast<float>(feedbackRate()), 1.0f);
        _combFilters.setAmount<3u>(static_cast<float>(feedbackRate()), 1.0f);
        _combFilters.sendDataAll(_inputCache.data<float>(), _inputCache.size<float>());
    }
}
