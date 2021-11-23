/**
 * @file CutoffFilter.ipp
 * @brief CutoffFilter implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::CutoffFilter::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _filter.init(
        DSP::Filter::FIRSpecs(
            static_cast<DSP::Filter::BasicType>(filterType()),
            DSP::Filter::WindowType::Hanning,
            33ul,
            static_cast<float>(audioSpecs().sampleRate),
            static_cast<float>(cutoffFrequency()),
            0.0f,
            1.0f
        )
    );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::CutoffFilter::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    if (static_cast<bool>(bypass())) {
        std::memcpy(out, _cache.data<float>(), output.size<std::uint8_t>());
        return;
    }

    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    // _filter._setGain(ConvertDecibelToRatio(static_cast<DB>(toto())));
    const float cutoffRate = static_cast<float>((cutoffFrequency() - 50.0) / 22'000.0);
    _filter.setSpecs(
        DSP::Filter::FIRSpecs(
            static_cast<DSP::Filter::BasicType>(filterType()),
            DSP::Filter::WindowType::Default,
            33ul,
            static_cast<float>(audioSpecs().sampleRate),
            // static_cast<float>(cutoffFrequency()),
            Utils::Log2<50, 22'000>::GetLog(cutoffRate),
            0.0f,
            1.0f
        )
    );
    _filter.filter(_cache.data<float>(), audioSpecs().processBlockSize, out, outGain);
}

inline void Audio::CutoffFilter::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(bypass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _cache, inGain, true);
}
