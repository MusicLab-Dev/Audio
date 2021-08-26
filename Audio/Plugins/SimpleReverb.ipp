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
    _inputCache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);

    _combFilters.reset<0u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 17.0f / 1000.0f);
    _combFilters.reset<1u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 29.0f / 1000.0f);
    _combFilters.reset<2u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 37.0f / 1000.0f);
    _combFilters.reset<3u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 47.0f / 1000.0f);
    _allPassFilters[0u].reset(audioSpecs(), 1.0f, 3.1f / 1000.0f);
    _allPassFilters[1u].reset(audioSpecs(), 1.0f, 1.7f / 1000.0f);
    _allPassFilters[2u].reset(audioSpecs(), 1.0f, 0.3f / 1000.0f);
}

inline void Audio::SimpleReverb::onAudioGenerationStarted(const BeatRange &)
{
    _inputCache.clear();

    _combFilters.reset<0u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 17.0f / 1000.0f);
    _combFilters.reset<1u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 29.0f / 1000.0f);
    _combFilters.reset<2u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 37.0f / 1000.0f);
    _combFilters.reset<3u>(audioSpecs(), 10.0f, static_cast<float>(reverbTime()) * 47.0f / 1000.0f);
    _allPassFilters[0u].reset(audioSpecs(), 1.0f, 3.1f / 1000.0f);
    _allPassFilters[1u].reset(audioSpecs(), 1.0f, 1.7f / 1000.0f);
    _allPassFilters[2u].reset(audioSpecs(), 1.0f, 0.3f / 1000.0f);
}

inline void Audio::SimpleReverb::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    const auto outSize = output.size<float>();

    output.clear();
    if (static_cast<bool>(bypass())) {
        std::memcpy(out, _inputCache.data<float>(), output.size<std::uint8_t>());
        return;
    }

    _combFilters.setInternalRate<0u>(1.0f, 1.0f);
    _combFilters.setInternalRate<1u>(1.0f, 1.0f);
    _combFilters.setInternalRate<2u>(1.0f, 1.0f);
    _combFilters.setInternalRate<3u>(1.0f, 1.0f);
    _combFilters.setFeedbackAmount<0u>(static_cast<float>(combFeedback()));
    _combFilters.setFeedbackAmount<1u>(static_cast<float>(combFeedback()));
    _combFilters.setFeedbackAmount<2u>(static_cast<float>(combFeedback()));
    _combFilters.setFeedbackAmount<3u>(static_cast<float>(combFeedback()));
    _combFilters.setDelayTime<0u>(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(reverbTime()) * 17.0f / 1000.0f);
    _combFilters.setDelayTime<1u>(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(reverbTime()) * 29.0f / 1000.0f);
    _combFilters.setDelayTime<2u>(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(reverbTime()) * 37.0f / 1000.0f);
    _combFilters.setDelayTime<3u>(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(reverbTime()) * 47.0f / 1000.0f);

    _allPassFilters[0u].setInternalRate(1.0f, 1.0f);
    _allPassFilters[1u].setInternalRate(1.0f, 1.0f);
    _allPassFilters[2u].setInternalRate(1.0f, 1.0f);
    _allPassFilters[0u].setFeedbackAmount(static_cast<float>(allPassFeedback()));
    _allPassFilters[1u].setFeedbackAmount(static_cast<float>(allPassFeedback()));
    _allPassFilters[2u].setFeedbackAmount(static_cast<float>(allPassFeedback()));

    _allPassFilters[0u].setDelayTime(static_cast<float>(audioSpecs().sampleRate), 3.1f / 1000.0f);
    _allPassFilters[1u].setDelayTime(static_cast<float>(audioSpecs().sampleRate), 1.7f / 1000.0f);
    _allPassFilters[2u].setDelayTime(static_cast<float>(audioSpecs().sampleRate), 0.3f / 1000.0f);


    // _combFilters.sendDataAll(out, outSize);
    _combFilters.receiveDataAll<false>(out, outSize, 0.7f);
    // std::memcpy(out, _inputCache.data<float>(), outSize * sizeof(float));
    _allPassFilters[0u].sendData(out, outSize);
    _allPassFilters[0u].receiveData<false>(out, outSize, 0.7f);
    // _allPassFilters[1u].sendData(out, outSize);
    // _allPassFilters[1u].receiveData<false>(out, outSize, 0.7f);
    // allPass[2u].sendData(out, outSize);
    // allPass[2u].receiveData<false>(out, outSize, 0.7f);


    const auto realMixRate = static_cast<float>(mixRate());
    float dry { 1.0f };
    float wet { 1.0f };
    if (realMixRate != 0.5f) {
        if (realMixRate >= 0.5f) {
            wet = (1.0f - realMixRate) * 2.0f;
        } else {
            dry = (realMixRate * 2.0f);
        }
    }
    for (auto i = 0u; i < outSize; ++i) {
        const auto delayOut = out[i];
        // UNUSED(wet);
        // UNUSED(dry);
        out[i] = _inputCache.data<float>()[i] * dry + delayOut * wet;
    }


}

inline void Audio::SimpleReverb::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(bypass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _inputCache, inGain, true);
    if (!static_cast<bool>(bypass())) {
        _combFilters.sendDataAll(_inputCache.data<float>(), _inputCache.size<float>());
    }
}
