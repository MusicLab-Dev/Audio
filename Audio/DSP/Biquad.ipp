/**
 * @ Author: Pierre Veysseyre
 * @ Description: Biquad implementation
 */

#include <iostream>
#include <chrono>

template<>
template<typename Type>
inline Type Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Direct1>::processSample(const Type in, const Key key, const Audio::DB outGain) noexcept
{
    float out = _coefs.b[0] * in + _coefs.b[1] * _regs[key][0] + _coefs.b[2] * _regs[key][1] -
                _coefs.a[1] * _regs[key][2] - _coefs.a[2] * _regs[key][3];

    // Shift registers
    _regs[key][1] = _regs[key][0];
    _regs[key][0] = in;
    _regs[key][3] = _regs[key][2];
    _regs[key][2] = out;
    return out * outGain;
}

template<>
template<typename Type>
void Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Direct1>::filterBlock(const Type *input, const std::size_t inputSize, Type *output, const Key key, const Audio::DB outGain) noexcept
{
    for (auto i = 0ul; i < inputSize; ++i) {
        *output++ = processSample(input[i], key, outGain);
    }
}

template<>
template<>
inline float Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::processSample(const float in, const Key key, const Audio::DB outGain) noexcept
{
    const float out = in * _coefs.a[0] + _regs[key][0];

    _regs[key][0] = in * _coefs.a[1] + _regs[key][1] - _coefs.b[1] * out;
    _regs[key][1] = in * _coefs.a[2] - _coefs.b[2] * out;
    return out * outGain;
}

// template<>
// inline float Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::process1(const float in) noexcept
// {
//     const float out = _regs[key][0] + in * _coefs.b[0];

//     _regs[key][0] = _regs[key][1] + in * _coefs.b[1] - (_coefs.a[1] * out);
//     _regs[key][1] = in * _coefs.b[2] - (_coefs.a[2] * out);
//     return out;
// }

template<>
template<>
inline void Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::filterBlock(const float *input, const std::size_t inputSize, float *output, const Key key, const Audio::DB outGain) noexcept
{
    // processSample(input[0]);
    // processSample(input[1]);
    // len -= 2;
    // input += 2;
    for (auto i = 0ul; i < inputSize; ++i) {
        *output++ = processSample(input[i], key, outGain);
    }
    // *input = processSample(0.f);
    // ++input;
    // *input = processSample(0.f);
}

// template<>
// inline void Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::processBlock1(float *block, std::size_t len) noexcept
// {
//     for (; len; --len, ++block)
//         *block = process1(*block);

//     //     const auto in = *block;
//     //     const float out = in * _coefs.a[0] + _regs[key][0];
//     //     _regs[key][0] = in * _coefs.a[1] + _regs[key][1] - _coefs.b[1] * out;
//     //     _regs[key][1] = in * _coefs.a[2] - _coefs.b[2] * out;
//     //     *block = out;
//     // }
// }


inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::GenerateCoefficients(const Internal::Specs &specs) noexcept
{
    switch (specs.filterType) {
    case DSP::Filter::AdvancedType::LowPass:
        return Internal::GenerateCoefficientsLowPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    case DSP::Filter::AdvancedType::HighPass:
        return Internal::GenerateCoefficientsHighPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    case DSP::Filter::AdvancedType::BandPass:
        return Internal::GenerateCoefficientsBandPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    case DSP::Filter::AdvancedType::BandPassFlat:
        return Internal::GenerateCoefficientsBandPassFlat(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    case DSP::Filter::AdvancedType::BandStop:
        return Internal::GenerateCoefficientsBandStop(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    case DSP::Filter::AdvancedType::Peak:
        return Internal::GenerateCoefficientsPeak(specs.sampleRate, specs.cutoffs[0], specs.gain, specs.qFactor);
    case DSP::Filter::AdvancedType::LowShelf:
        return Internal::GenerateCoefficientsLowShelf(specs.sampleRate, specs.cutoffs[0], specs.gain, specs.qFactor);
    case DSP::Filter::AdvancedType::HighShelf:
        return Internal::GenerateCoefficientsHighShelf(specs.sampleRate, specs.cutoffs[0], specs.gain, specs.qFactor);
    case DSP::Filter::AdvancedType::AllPass:
        return Internal::GenerateCoefficientsAllPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    default:
        return Internal::GenerateCoefficientsLowPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor);
    }
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsLowPass(const float sampleRate, const float freq, const float q) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float alpha = omegaSin / (2.0f * q);

    Internal::Coefficients coefs {};
    coefs.b[0] = (1.0f - omegaCos) / 2.0f;
    coefs.b[1] = 1.0f - omegaCos;
    coefs.b[2] = (1.0f - omegaCos) / 2.0f;
    coefs.a[0] = 1.0f + alpha;
    coefs.a[1] = -2.0f * omegaCos;
    coefs.a[2] = 1.0f - alpha;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsHighPass(const float sampleRate, const float freq, const float q) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float alpha = omegaSin / (2.0f * q);

    Internal::Coefficients coefs {};
    coefs.b[0] = (1.0f + omegaCos) / 2.0f;
    coefs.b[1] = -(1.0f + omegaCos);
    coefs.b[2] = (1.0f + omegaCos) / 2.0f;
    coefs.a[0] = 1.0f + alpha;
    coefs.a[1] = -2.0f * omegaCos;
    coefs.a[2] = 1.0f - alpha;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsBandPass(const float sampleRate, const float freq, const float q) noexcept
{
    UNUSED(q);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float alpha = omegaSin / 2.0f;

    Internal::Coefficients coefs {};
    coefs.b[0] = alpha;
    coefs.b[1] = 0;
    coefs.b[2] = -alpha;
    coefs.a[0] = 1 + alpha;
    coefs.a[1] = -2 * omegaCos;
    coefs.a[2] = 1 - alpha;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsBandPassFlat(const float sampleRate, const float freq, const float q) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float alpha = omegaSin / (2.0f * q);

    Internal::Coefficients coefs {};
    coefs.b[0] = alpha;
    coefs.b[1] = 0;
    coefs.b[2] = -alpha;
    coefs.a[0] = 1 + alpha;
    coefs.a[1] = -2 * omegaCos;
    coefs.a[2] = 1 - alpha;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsBandStop(const float sampleRate, const float freq, const float q) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float alpha = omegaSin / (2.0f * q);

    Internal::Coefficients coefs {};
    coefs.b[0] = 1;
    coefs.b[1] = -2 * omegaCos;
    coefs.b[2] = 1;
    coefs.a[0] = 1 + alpha;
    coefs.a[1] = -2 * omegaCos;
    coefs.a[2] = 1 - alpha;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsPeak(const float sampleRate, const float freq, const float gain, const float q) noexcept
{
    const float amp = std::pow(10.0f, gain / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float alpha = omegaSin / (2.0f * q);
    const float alphaTimeAmp = alpha * amp;
    const float alphaOverAmp = alpha / amp;

    Internal::Coefficients coefs {};
    coefs.b[0] = 1 + alphaTimeAmp;
    coefs.b[1] = -2 * omegaCos;
    coefs.b[2] = 1 - alphaTimeAmp;
    coefs.a[0] = 1 + alphaOverAmp;
    coefs.a[1] = -2 * omegaCos;
    coefs.a[2] = 1 - alphaOverAmp;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsLowShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept
{
    const float amp = std::pow(10.0f, gain / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float beta = std::sqrt(amp) / q;

    const float ampPlusOne = amp + 1.0f;
    const float ampMinusOne = amp - 1.0f;
    const float minusOmegaCos = ampMinusOne * omegaCos;
    const float betaTimeOmegaSin = beta * omegaSin;

    Internal::Coefficients coefs {};
    coefs.b[0] = amp * (ampPlusOne - minusOmegaCos + betaTimeOmegaSin);
    coefs.b[1] = 2.0f * amp * (ampMinusOne - ampPlusOne * omegaCos);
    coefs.b[2] = amp * (ampPlusOne - minusOmegaCos - betaTimeOmegaSin);
    coefs.a[0] = ampPlusOne + minusOmegaCos + betaTimeOmegaSin;
    coefs.a[1] = -2.0f * (ampMinusOne + ampPlusOne * omegaCos);
    coefs.a[2] = ampPlusOne + minusOmegaCos - betaTimeOmegaSin;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsHighShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept
{
    const float amp = std::pow(10.0f, gain / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaSin = std::sin(omega);
    const float omegaCos = std::cos(omega);
    const float beta = std::sqrt(amp) / q;

    Internal::Coefficients coefs {};
    coefs.b[0] = amp * ((amp + 1) + (amp - 1) * omegaCos + beta * omegaSin);
    coefs.b[1] = -2 * amp * ((amp - 1) + (amp + 1) * omegaCos);
    coefs.b[2] = amp * ((amp + 1) + (amp - 1) * omegaCos - beta * omegaSin);
    coefs.a[0] = (amp + 1) - (amp - 1) * omegaCos + beta * omegaSin;
    coefs.a[1] = 2 * ((amp - 1) - (amp + 1) * omegaCos);
    coefs.a[2] = (amp + 1) - (amp - 1) * omegaCos - beta * omegaSin;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsAllPass(const float sampleRate, const float freq, const float q) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float omegaCos = 2.0f * std::cos(omega);
    const float omegaSin = std::sin(omega);
    const float alpha = omegaSin / (2.0f * q);

    const float oneMinusAlpha = 1.0f - alpha;
    const float onePlusAlpha = 1.0f + alpha;

    Internal::Coefficients coefs {};
    coefs.b[0] = oneMinusAlpha;
    coefs.b[1] = -omegaCos;
    coefs.b[2] = onePlusAlpha;
    coefs.a[0] = onePlusAlpha;
    coefs.a[1] = -omegaCos;
    coefs.a[2] = oneMinusAlpha;

    coefs.b[0] /= coefs.a[0];
    coefs.b[1] /= coefs.a[0];
    coefs.b[2] /= coefs.a[0];
    coefs.a[1] /= coefs.a[0];
    coefs.a[2] /= coefs.a[0];

    return coefs;
}
