/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/DSP/Gain.hpp>
#include <Audio/DSP/Merge.hpp>
#include <Audio/DSP/FIR.hpp>

inline void Audio::SimpleDelay::onAudioParametersChanged(void)
{
    _delay.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
    _inputCache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
}

inline void Audio::SimpleDelay::onAudioGenerationStarted(const BeatRange &)
{
    _inputCache.clear();
    _delay.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
}

inline void Audio::SimpleDelay::receiveAudio(BufferView output)
{
    const float *in = _inputCache.data<float>();
    float *out = output.data<float>();
    const auto outSize = output.size<float>();
    const float outGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float outGainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    output.clear();
    if (static_cast<bool>(bypass())) {
        std::memcpy(out, in, output.size<std::uint8_t>());
        DSP::Gain::Apply<float>(out, outSize, outGainFrom, outGainTo);
        return;
    }

    _delay.setInputAmount(0.0f);
    _delay.setDelayAmount(static_cast<float>(feedbackAmount()));
    _delay.setDelayTime(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(delayTime()));

    _delay.process<0u, false>(_inputCache.data<float>(), out, outSize);

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
        out[i] = in[i] * dry + delayOut * wet;
    }

    DSP::Gain::Apply<float>(out, outSize, outGainFrom, outGainTo);
}

inline void Audio::SimpleDelay::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    DSP::Merge<float>(inputs, _inputCache, 1.0f, false);

    const float inGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u)));
    const float inGainTo = ConvertDecibelToRatio(static_cast<float>(inputGain()));

    DSP::Gain::Apply<float>(_inputCache.data<float>(), _inputCache.size<float>(), inGainFrom, inGainTo);
}
