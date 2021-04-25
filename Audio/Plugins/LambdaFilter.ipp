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
    _filter.init(
        DSP::Filter::FIRSpec {
            static_cast<DSP::Filter::BasicType>(filterType()),
            DSP::Filter::WindowType::Hanning,
            255ul,
            static_cast<double>(audioSpecs().sampleRate),
            { cutoffFrequencyFrom(), cutoffFrequencyTo() },
            1.0
        }
    );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::LambdaFilter::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    output.clear();
    // Update filter cutoffs
    _filter.setCutoffs(cutoffFrequencyFrom(), cutoffFrequencyTo());
    _filter.filter(_cache.data<float>(), audioSpecs().processBlockSize, out);
    // std::memcpy(out, _cache.data<float>(), audioSpecs().processBlockSize * GetFormatByteLength(audioSpecs().format));
}

inline void Audio::LambdaFilter::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
}
