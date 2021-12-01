/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/DSP/Gain.hpp>
#include <Audio/DSP/Merge.hpp>

inline void Audio::SimpleReverb::onAudioParametersChanged(void)
{
    _inputCache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);

    _reverb.reset(audioSpecs());
}

inline void Audio::SimpleReverb::onAudioGenerationStarted(const BeatRange &)
{
    _inputCache.clear();

    _reverb.reset(audioSpecs());
}

inline void Audio::SimpleReverb::receiveAudio(BufferView output)
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

    _reverb.setReverbDuration(static_cast<float>(reverbTime()));
    _reverb.setPreDelayDuration(static_cast<float>(preDelayTime()));
    _reverb.process(in, out, outChannelSize);

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
    for (auto i = 0u; i < outChannelSize; ++i) {
        const auto delayOut = out[i];
        out[i] = in[i] * dry + delayOut * wet;
    }

    DSP::Gain::Apply<float>(out, outChannelSize, outGainFrom, outGainTo);
    if (channels - 1u) {
        std::memcpy(out + outChannelSize, out, outChannelSize * sizeof(float));
    }
}

inline void Audio::SimpleReverb::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    DSP::Merge<float>(inputs, _inputCache, 1.0f, false);

    const float inGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u)));
    const float inGainTo = ConvertDecibelToRatio(static_cast<float>(inputGain()));

    DSP::Gain::Apply<float>(_inputCache.data<float>(), _inputCache.size<float>(), inGainFrom, inGainTo);
}
