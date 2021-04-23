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
    _firFilter.setSpecs(DSP::Filter::FIRSpecs {
        static_cast<DSP::Filter::BasicType>(filterType()),
        DSP::Filter::WindowType::Hanning,
        255ul,
        static_cast<float>(audioSpecs().sampleRate),
        { static_cast<float>(cutoffFrequencyFrom()), static_cast<float>(cutoffFrequencyTo()) }
    });
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::LambdaFilter::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    // _firFilter.filter<true>(_cache.data<float>(), audioSpecs().processBlockSize, out);
    std::memcpy(out, _cache.data<float>(), audioSpecs().processBlockSize * GetFormatByteLength(audioSpecs().format));
}

inline void Audio::LambdaFilter::sendAudio(const BufferViews &inputs)
{
    if (const auto inputSize = inputs.size(); inputSize) {
        const auto size = inputs[0].size<float>();
        DSP::Merge<float>(inputs, _cache, size, true);
    }
}
