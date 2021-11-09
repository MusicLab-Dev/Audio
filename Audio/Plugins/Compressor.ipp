/**
 * @file Compressor.ipp
 * @brief Compressor implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::Compressor::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();

    _compressor.inputLevel = 0.0f;
    _compressor.envelopeLevel = 0.0f;
}

inline void Audio::Compressor::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    if (static_cast<bool>(bypass())) {
        std::memcpy(out, _cache.data<float>(), output.size<std::uint8_t>());
        return;
    }

    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()));
    const float *input = _cache.data<float>();
    const auto outSize = output.size<float>();

    _compressor.attackRate = 1000.0f / (static_cast<float>(audioSpecs().sampleRate) * attack());
    _compressor.releaseRate = 1000.0f / (static_cast<float>(audioSpecs().sampleRate) * release());

    const float thld = static_cast<float>(threshold());
    const float rt = static_cast<float>(ratio());

    auto &envLevel = _compressor.envelopeLevel;
    auto &inLevel = _compressor.inputLevel;

    float thresholdDelta { 0.0f };
    float gainReduction { 0.0f };
    for (auto i = 0u; i < outSize; ++i) {
        const auto in = input[i];
        inLevel = ConvertSampleToDecibel(std::abs(in));
        thresholdDelta = inLevel - thld;

        // if (inLevel)
            // std::cout << "\n- input: " << inLevel << ", thresholdDelta: " << thresholdDelta << std::endl;
        if (thresholdDelta > 0.0f) {
            _compressor.thresholdDelta = thresholdDelta;
            // Attack phase
            envLevel = std::min(1.0f, envLevel + _compressor.attackRate);
            // if (envLevel)
            //     std::cout << "/ " << envLevel << std::endl;
        } else {
            // Release phase
            _compressor.thresholdDelta += thresholdDelta;
            envLevel = std::max(0.0f, envLevel - _compressor.releaseRate);
            // if (envLevel)
            //     std::cout << "\\ " << envLevel << std::endl;
        }
        if (envLevel && _compressor.thresholdDelta) {
            // std::cout << "--> reduction: " << gainReduction << " :: " << ConvertDecibelToRatio(-gainReduction) << " :: " << envLevel << std::endl;
            gainReduction = envLevel * _compressor.thresholdDelta / rt;
        }
        else
            gainReduction = 0.0f;
        out[i] = in * ConvertDecibelToRatio(-gainReduction) * outGain;
    }
}

inline void Audio::Compressor::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(bypass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _cache, inGain, true);
}
