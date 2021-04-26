/**
 * @file GammaEqualizer.ipp
 * @brief Gamma equalize implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::GammaEqualizer::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _filter.init(
        DSP::Filter::WindowType::Hanning,
        255ul,
        static_cast<float>(audioSpecs().sampleRate),
        32.0f
    );
    // _filter.setGains({

    // })
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::GammaEqualizer::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    output.clear();
    _filter.filter(_cache.data<float>(), audioSpecs().processBlockSize, out, {
        ConvertDecibelToRatio(static_cast<float>(frequenyBands_0())),
        ConvertDecibelToRatio(static_cast<float>(frequenyBands_1()))
        // ConvertDecibelToRatio(frequenyBands_2()),
        // ConvertDecibelToRatio(frequenyBands_3()),
        // ConvertDecibelToRatio(frequenyBands_4()),
        // ConvertDecibelToRatio(frequenyBands_5()),
        // ConvertDecibelToRatio(frequenyBands_6()),
        // ConvertDecibelToRatio(frequenyBands_7()),
        // ConvertDecibelToRatio(frequenyBands_8()),
        // ConvertDecibelToRatio(frequenyBands_9())
    });
    // std::memcpy(out, _cache.data<float>(), audioSpecs().processBlockSize * GetFormatByteLength(audioSpecs().format));
}

inline void Audio::GammaEqualizer::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
}
