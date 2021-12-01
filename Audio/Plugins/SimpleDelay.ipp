/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/DSP/Gain.hpp>
#include <Audio/DSP/Merge.hpp>
#include <Audio/DSP/FIR.hpp>

inline void Audio::SimpleDelay::onAudioParametersChanged(void)
{
    _delayLeft.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
    _delayRight.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
    _inputCache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
}

inline void Audio::SimpleDelay::onAudioGenerationStarted(const BeatRange &)
{
    _inputCache.clear();
    _delayLeft.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
    _delayRight.reset(audioSpecs(), 10.0f, static_cast<float>(delayTime()));
}

inline void Audio::SimpleDelay::receiveAudio(BufferView output)
{
    const float *in = _inputCache.data<float>();
    float *out = output.data<float>();

    const auto outChannelSize = audioSpecs().processBlockSize;
    const auto channels = static_cast<std::size_t>(audioSpecs().channelArrangement);

    const float outGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float outGainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    output.clear();
    if (static_cast<bool>(bypass())) {
        std::memcpy(out, in, output.size<std::uint8_t>());
        DSP::Gain::Apply<float>(out, outChannelSize, outGainFrom, outGainTo);
        return;
    }

    _delayLeft.setInputAmount(0.0f);
    _delayLeft.setDelayAmount(static_cast<float>(feedbackAmount()));
    _delayLeft.setDelayTime(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(delayTime()));
    _delayRight.setInputAmount(0.0f);
    _delayRight.setDelayAmount(static_cast<float>(feedbackAmount()));
    _delayRight.setDelayTime(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(delayTime()));

    const auto gainStereo = 1.0f / static_cast<float>(channels);
    const auto shiftStereo = outChannelSize * (channels - 1u);

    _delayLeft.process<0u, false>(in, out, outChannelSize, gainStereo);
    _delayRight.process<0u, false>(in + shiftStereo, out + shiftStereo, outChannelSize, gainStereo);

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
    auto i = 0u;
    for (; i < outChannelSize; ++i) {
        const auto delayOut = out[i];
        out[i] = in[i] * dry + delayOut * wet;
    }
    const auto size = outChannelSize + shiftStereo;
    for (; i < size; ++i) {
        const auto delayOut = out[i];
        out[i] = in[i] * dry + delayOut * wet;
    }


    DSP::Gain::ApplyStereo<float>(out, outChannelSize, channels, outGainFrom, outGainTo);
}

inline void Audio::SimpleDelay::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    DSP::Merge<float>(inputs, _inputCache, 1.0f, false);

    const float inGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u)));
    const float inGainTo = ConvertDecibelToRatio(static_cast<float>(inputGain()));

    DSP::Gain::ApplyStereo<float>(
        _inputCache.data<float>(),
        audioSpecs().processBlockSize,
        static_cast<std::size_t>(audioSpecs().channelArrangement),
        inGainFrom,
        inGainTo
    );
}
