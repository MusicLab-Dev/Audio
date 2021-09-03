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
    _lastOut = 0.0f;
    _filter.setup(DSP::Biquad::Internal::Specs {
        static_cast<DSP::Filter::AdvancedType>(filterType()),
        static_cast<float>(audioSpecs().sampleRate),
        { static_cast<float>(cutoffFrequencyFrom()), static_cast<float>(cutoffFrequencyTo()) },
        1.0f,
        0.707f
    });
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::SigmaFilter::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<DB>(outputVolume()));
    float *out = output.data<float>();
    output.clear();

    _filter.setup(DSP::Biquad::Internal::Specs {
        static_cast<DSP::Filter::AdvancedType>(filterType()),
        static_cast<float>(audioSpecs().sampleRate),
        { static_cast<float>(cutoffFrequencyFrom()), static_cast<float>(cutoffFrequencyTo()) },
        1.0f,
        0.707f
    });
    _filter.filterBlock(_cache.data<float>(), audioSpecs().processBlockSize, out, 0u, outGain);


    // const auto outSize = output.size<float>();
    // const float cutOffNorm = GetFrequencyNorm(static_cast<float>(cutoffFrequencyFrom()), audioSpecs().sampleRate);
    // const float decay = std::pow(M_Ef32, -cutOffNorm);
    // const auto a = decay;
    // const auto b = 1.0f - a;

    // const float *in = _cache.data<float>();
    // for (auto i = 0u; i < outSize; ++i) {
    //     _lastOut += b * (in[i] - _lastOut);
    //     out[i] = _lastOut;
    // }
}

inline void Audio::SigmaFilter::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
}
