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
    _filter.init(
        DSP::Filter::WindowType::Hanning,
        255ul,
        static_cast<double>(audioSpecs().sampleRate)
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
        0, 1
    });
    // std::memcpy(out, _cache.data<float>(), audioSpecs().processBlockSize * GetFormatByteLength(audioSpecs().format));
}

inline void Audio::GammaEqualizer::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
}
