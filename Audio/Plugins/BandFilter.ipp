/**
 * @file BandFilter.ipp
 * @brief BandFilter implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::BandFilter::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _filter.init(
        DSP::Filter::FIRSpecs(
            DSP::Filter::BasicType::BandPass,
            DSP::Filter::WindowType::Hanning,
            FilterSize,
            static_cast<float>(audioSpecs().sampleRate),
            static_cast<float>(cutoffFrequencyFrom()),
            static_cast<float>(cutoffFrequencyTo()),
            1.0f
        )
    );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();

    _cacheStereoFilter.resize(FilterSize - 1u);
    _cacheStereoFilter.clear();
}

inline void Audio::BandFilter::receiveAudio(BufferView output)
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


    // Doesn't sound cool anymore with Warmup project
    const float cutoffFromRate = static_cast<float>((cutoffFrequencyFrom() - 50.0) / 22'000.0);
    const float cutoffToRate = static_cast<float>((cutoffFrequencyTo() - 50.0) / 22'000.0);

    _filter.setSpecs(
        DSP::Filter::FIRSpecs(
            static_cast<DSP::Filter::BasicType>(filterType() + static_cast<ParamValue>(DSP::Filter::BasicType::BandPass)),
            DSP::Filter::WindowType::Default,
            FilterSize,
            static_cast<float>(audioSpecs().sampleRate),
            Utils::LogFrequency::GetLog(cutoffFromRate),
            Utils::LogFrequency::GetLog(cutoffToRate),
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

inline void Audio::BandFilter::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(bypass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _cache, inGain, true);
}
