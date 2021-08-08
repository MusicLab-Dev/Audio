/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

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
}

inline void Audio::SimpleDelay::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    const auto outSize = output.size<float>();

    output.clear();
    if (static_cast<bool>(byBass())) {
        std::memcpy(out, _inputCache.data<float>(), output.size<std::uint8_t>());
        return;
    }

    /** @todo Maybe add this control to the process (_delay.receiveData) */
    _delay.setDelayTime(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(delayTime()));
    // Mix input & delay output
    _delay.receiveData(out, outSize, static_cast<float>(mixRate()));

    // float dry { 1.0f };
    // float wet { 1.0f };
    // if (mixRate != 0.5f) {
    //     if (mixRate >= 0.5f) {
    //         wet = 1.0f - mixRate;
    //     } else {
    //         dry = mixRate;
    //     }
    // }
    // for (auto i = 0u; i < outSize; ++i) {
    //     const auto delayOut = out[i];
    //     out[i] = _inputCache.data<float>()[i] * dry + delayOut * wet;
    // }

}

inline void Audio::SimpleDelay::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(byBass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _inputCache, inGain, true);
    if (!static_cast<bool>(byBass())) {
        _delay.sendData(_inputCache.data<float>(), _inputCache.size<float>(), static_cast<float>(feedbackRate()));
    }
}
