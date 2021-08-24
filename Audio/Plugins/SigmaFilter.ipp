/**
 * @file SigmaFilter.ipp
 * @brief Sigma filter implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::SigmaFilter::onAudioGenerationStarted(const BeatRange &)
{
    // _filter.init(
    //     DSP::Filter::FIRSpecs {
    //         static_cast<DSP::Filter::BasicType>(filterType()),
    //         DSP::Filter::WindowType::Hanning,
    //         255ul,
    //         static_cast<double>(audioSpecs().sampleRate),
    //         { cutoffFrequencyFrom(), cutoffFrequencyTo() },
    //         1.0
    //     }
    // );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::SigmaFilter::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<DB>(outputVolume()));
    float *out = output.data<float>();
    output.clear();
    // Update filter cutoffs
    // _filter.setCutoffs(cutoffFrequencyFrom(), cutoffFrequencyTo());

    _filter.filterBlock(_cache.data<float>(), audioSpecs().processBlockSize, out, 0u, outGain);
}

inline void Audio::SigmaFilter::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
    // PrintRangeClip(_cache);
}
