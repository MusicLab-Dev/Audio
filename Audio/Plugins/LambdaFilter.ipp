/**
 * @file LambdaFilter.ipp
 * @brief Lambda filter implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::LambdaFilter::onAudioGenerationStarted(const BeatRange &range)
{
    const DSP::Filter::FIRSpecs specs {
        DSP::Filter::BasicType::LowPass,
        DSP::Filter::WindowType::Hanning,
        255ul,
        44100,
        { 0.1, 0.1 }
    };

    _firFilter.setSpecs(specs);
    std::cout << static_cast<float>(cutoffFrequencyFrom() * audioSpecs().sampleRate / 2.0f) << std::endl;
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::LambdaFilter::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    output.clear();
    _firFilter.filter<true>(_cache.data<float>(), audioSpecs().processBlockSize, out);
    // std::memcpy(out, _cache.data<float>(), audioSpecs().processBlockSize * GetFormatByteLength(audioSpecs().format));
}

inline void Audio::LambdaFilter::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
}
