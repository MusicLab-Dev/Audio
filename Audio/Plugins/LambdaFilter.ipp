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
constexpr auto PrintRangeClip = [](const Audio::BufferView buffer) {
    const auto size = buffer.size<float>();
    const float *out = buffer.data<float>();
    auto min = 1000.f;
    auto max = -1000.f;
    for (auto i = 0ul; i < size; ++i) {
        min = std::min(out[i], min);
        max = std::max(out[i], max);
    }
    if (min <= -1 || max >= 1)
        std::cout << "<<<CLIPING>>>" << std::endl;
    else
        std::cout << std::setprecision(4) << "min: " << min << ", max: " << max << std::endl;
};

inline void Audio::LambdaFilter::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(outputVolume());
    float *out = output.data<float>();
    output.clear();
    // Update filter cutoffs
    _filter.setCutoffs(cutoffFrequencyFrom(), cutoffFrequencyTo());
    // PrintRangeClip(output);
    // std::cout << "=====" << std::endl;
    _filter.filter(_cache.data<float>(), audioSpecs().processBlockSize, out, outGain);
    // PrintRangeClip(output);
    // std::memcpy(out, _cache.data<float>(), audioSpecs().processBlockSize * GetFormatByteLength(audioSpecs().format));
}

inline void Audio::LambdaFilter::sendAudio(const BufferViews &inputs)
{
    if (inputs.size()) {
        DSP::Merge<float>(inputs, _cache, true);
    }
    // PrintRangeClip(_cache);
}
