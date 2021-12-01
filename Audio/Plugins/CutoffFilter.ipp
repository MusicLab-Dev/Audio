/**
 * @file CutoffFilter.ipp
 * @brief CutoffFilter implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Gain.hpp>
#include <Audio/DSP/Merge.hpp>

inline void Audio::CutoffFilter::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _filter.init(
        DSP::Filter::FIRSpecs(
            static_cast<DSP::Filter::BasicType>(filterType()),
            DSP::Filter::WindowType::Hanning,
            FilterSize,
            static_cast<float>(audioSpecs().sampleRate),
            static_cast<float>(cutoffFrequency()),
            0.0f,
            1.0f
        )
    );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();

    _cacheStereoFilter.resize(FilterSize - 1u);
    _cacheStereoFilter.clear();
}

inline void Audio::CutoffFilter::receiveAudio(BufferView output)
{
    const float *in = _cache.data<float>();
    float *out = output.data<float>();
    const auto channels = static_cast<std::size_t>(audioSpecs().channelArrangement);
    const auto outChannelSize = audioSpecs().processBlockSize;
    const float outGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float outGainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    if (static_cast<bool>(bypass())) {
        std::memcpy(out, in, outChannelSize * channels * sizeof(float));
        DSP::Gain::ApplyStereo<float>(out, outChannelSize, channels, outGainFrom, outGainTo);
        return;
    }

    // _filter._setGain(ConvertDecibelToRatio(static_cast<DB>(toto())));
    const float cutoffRate = static_cast<float>((cutoffFrequency() - 50.0) / 22'000.0);
    _filter.setSpecs(
        DSP::Filter::FIRSpecs(
            static_cast<DSP::Filter::BasicType>(filterType()),
            DSP::Filter::WindowType::Default,
            FilterSize,
            static_cast<float>(audioSpecs().sampleRate),
            Utils::LogFrequency::GetLog(cutoffRate),
            0.0f,
            1.0f
        )
    );

    // Left
    _filter.filter(in, outChannelSize, out);

    // Right
    if (channels - 1u) {
        std::swap(_cacheStereoFilter, _filter.lastInput());
        _filter.filter(in + outChannelSize, outChannelSize, out + outChannelSize);
        std::swap(_cacheStereoFilter, _filter.lastInput());
    }
    DSP::Gain::ApplyStereo<float>(out, outChannelSize, channels, outGainFrom, outGainTo);
}

inline void Audio::CutoffFilter::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    DSP::Merge<float>(inputs, _cache, 1.0f, false);

    const float inGainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u)));
    const float inGainTo = ConvertDecibelToRatio(static_cast<float>(inputGain()));

    DSP::Gain::ApplyStereo<float>(
        _cache.data<float>(),
        audioSpecs().processBlockSize,
        static_cast<std::size_t>(audioSpecs().channelArrangement),
        inGainFrom,
        inGainTo
    );
}
