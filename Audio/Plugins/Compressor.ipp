/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>
#include <cmath>

#include <Audio/DSP/Merge.hpp>
#include <Audio/DSP/Gain.hpp>

inline void Audio::Compressor::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();

    _lastLevel = 0.0f;
    _levels.resize(audioSpecs().processBlockSize);
}

inline void Audio::Compressor::receiveAudio(BufferView output)
{
    const float gainOutFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(0u)));
    const float gainOutTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    const auto outSize = output.size<float>();
    auto *out = output.data<float>();
    const auto *in = _cache.data<float>();

    const auto slope = 1.0f / static_cast<float>(ratio()) - 1.0f;
    const auto thresholdF = static_cast<float>(threshold());

    // 1 - Level detection
    std::fill(_levels.begin(), _levels.end(), 0.0f);
    // Get levels
    for (auto i = 0u; i < outSize; ++i) {
        // Hard-knee
        if (const auto dt = ConvertSampleToDecibel(in[i]) - thresholdF; dt >= 0.0f)
            _levels[i] = dt * slope;
    }

    // 2 - Timing (attack/release)
    const auto sr = static_cast<float>(audioSpecs().sampleRate) / 1000.0f;
    const auto attackF = static_cast<float>(attack());
    const auto releaseF = static_cast<float>(release());
    const auto attackRate = std::exp(-1.0f / (attackF * sr));
    const auto releaseRate = std::exp(-1.0f / (releaseF * sr));
    const auto attackRateRev = 1.0f - attackRate;
    const auto releaseRateRev = 1.0f - releaseRate;

    // Smooth the levels
    for (auto i = 0u; i < outSize; ++i) {
        // Attack / release
        if (_levels[i] > _lastLevel) {
            _lastLevel = attackRate * _lastLevel + attackRateRev * _levels[i];
        }
        else {
            _lastLevel = releaseRate * _lastLevel + releaseRateRev * _levels[i];
        }
        if (smooth() == 1.0f)
            out[i] = ConvertDecibelToRatio(_lastLevel);
        else
            out[i] = _levels[i];
    }
    // std::cout << "\r" << _lastLevel << std::flush;
    // if (std::abs(_lastLevel) >= 0.001)
    //     std::cout << std::setprecision(2) << _lastLevel << std::endl;

    // auto bar = "[" + std::string(2, '#')

    // 3 - Apply gain reduction
    if (apply() == 1.0f) {
        for (auto i = 0u; i < outSize; ++i) {
            out[i] *= in[i];
        }
    }

    if (static_cast<bool>(bypass())) {
        std::memcpy(out, _cache.data<float>(), outSize * sizeof(float));
    }
    DSP::Gain::Apply<float>(out, outSize, gainOutFrom, gainOutTo);
}

inline void Audio::Compressor::sendAudio(const BufferViews &inputs)
{
    DSP::Merge<float>(inputs, _cache, 1.0f, false);
    DSP::Gain::Apply<float>(
        _cache.data<float>(),
        _cache.size<float>(),
        ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u))),
        ConvertDecibelToRatio(static_cast<float>(inputGain()))
    );
}
