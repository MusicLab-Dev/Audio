/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

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
    float *out = output.data<float>();
    const auto outSize = output.size<float>();

    output.clear();
    if (static_cast<bool>(bypass())) {
        std::memcpy(out, _inputCache.data<float>(), output.size<std::uint8_t>());
        return;
    }

    _reverb.setReverbDuration(static_cast<float>(reverbTime()));
    _reverb.setPreDelayDuration(static_cast<float>(preDelayTime()));
    _reverb.process(_inputCache.data<float>(), out, outSize);

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
}
