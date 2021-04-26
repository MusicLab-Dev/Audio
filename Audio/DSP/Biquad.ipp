/**
 * @ Author: Pierre Veysseyre
 * @ Description: Biquad implementation
 */

#include <iostream>
#include <chrono>

template<Audio::DSP::Biquad::Internal::Form Form>
inline void Audio::DSP::Biquad::Filter<Form>::resetRegisters(void) noexcept
{
    for (auto i = 0u; i < sizeof(_regs) / 4; ++i)
        _regs[i] = 0.0f;
}

template<>
template<typename Type>
inline Type Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Direct1>::process(const Type in, const Audio::DB outGain) noexcept
{
    float out = _coefs.b[0] * in + _coefs.b[1] * _regs[0] + _coefs.b[2] * _regs[1] -
                _coefs.a[1] * _regs[2] - _coefs.a[2] * _regs[3];

    // Shift registers
    _regs[1] = _regs[0];
    _regs[0] = in;
    _regs[3] = _regs[2];
    _regs[2] = out;
    return out * outGain;
}

template<>
template<typename Type>
void Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Direct1>::filterBlock(const Type *input, const std::size_t inputSize, Type *output, const Audio::DB outGain) noexcept
{
    for (auto i = 0ul; i < inputSize; ++i) {
        *output++ = process(input[i], outGain);
    }
}

template<>
template<>
inline float Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::process(const float in, const Audio::DB outGain) noexcept
{
    const float out = in * _coefs.a[0] + _regs[0];

    // return out;
    _regs[0] = in * _coefs.a[1] + _regs[1] - _coefs.b[1] * out;
    _regs[1] = in * _coefs.a[2] - _coefs.b[2] * out;
    return out * outGain;
}

// template<>
// inline float Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::process1(const float in) noexcept
// {
//     const float out = _regs[0] + in * _coefs.b[0];

//     _regs[0] = _regs[1] + in * _coefs.b[1] - (_coefs.a[1] * out);
//     _regs[1] = in * _coefs.b[2] - (_coefs.a[2] * out);
//     return out;
// }

template<>
template<>
inline void Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::filterBlock(const float *input, const std::size_t inputSize, float *output, const Audio::DB outGain) noexcept
{
    // process(input[0]);
    // process(input[1]);
    // len -= 2;
    // input += 2;
    for (auto i = 0ul; i < inputSize; ++i) {
        *output++ = process(input[i], outGain);
    }
    // *input = process(0.f);
    // ++input;
    // *input = process(0.f);
}

// template<>
// inline void Audio::DSP::Biquad::Filter<Audio::DSP::Biquad::Internal::Form::Transposed2>::processBlock1(float *block, std::size_t len) noexcept
// {
//     for (; len; --len, ++block)
//         *block = process1(*block);

//     //     const auto in = *block;
//     //     const float out = in * _coefs.a[0] + _regs[0];
//     //     _regs[0] = in * _coefs.a[1] + _regs[1] - _coefs.b[1] * out;
//     //     _regs[1] = in * _coefs.a[2] - _coefs.b[2] * out;
//     //     *block = out;
//     // }
// }


inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::GenerateCoefficients(const Internal::Specs &specs) noexcept
{
    switch (specs.filterType) {
    case DSP::Filter::AdvancedType::LowPass:
        return Internal::GenerateCoefficientsLowPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor, specs.qAsBandWidth);
    case DSP::Filter::AdvancedType::HighPass:
        return Internal::GenerateCoefficientsHighPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor, specs.qAsBandWidth);
    default:
        return Internal::GenerateCoefficientsLowPass(specs.sampleRate, specs.cutoffs[0], specs.qFactor, specs.qAsBandWidth);
    }
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsLowPass(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float alpha = qAsBandWidth ? (tsin * std::sinh(std::log(2.0f) / 2.0f * q * omega / tsin)) : (tsin / (2.0f * q));

    Internal::Coefficients coefs {};
    coefs.b[0] = (1.0f - tcos) / 2.0f;
    coefs.b[1] = 1.0f - tcos;
    coefs.b[2] = (1.0f - tcos) / 2.0f;
    coefs.a[0] = 1.0f + alpha;
    coefs.a[1] = -2.0f * tcos;
    coefs.a[2] = 1.0f - alpha;

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsHighPass(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float alpha = qAsBandWidth ? (tsin * std::sinh(std::log(2.0f) / 2.0f * q * omega / tsin)) : (tsin / (2.0f * q));

    Internal::Coefficients coefs {};
    coefs.b[0] = (1 + tcos) / 2;
    coefs.b[1] = -(1 + tcos);
    coefs.b[2] = (1 + tcos) / 2;
    coefs.a[0] = 1 + alpha;
    coefs.a[1] = -2 * tcos;
    coefs.a[2] = 1 - alpha;

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsBandPass(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float alpha = qAsBandWidth ? (tsin * std::sinh(std::log(2.0f) / 2.0f * q * omega / tsin)) : (tsin / (2.0f * q));

    Internal::Coefficients coefs {};
    coefs.b[0] = alpha;
    coefs.b[1] = 0;
    coefs.b[2] = -alpha;
    coefs.a[0] = 1 + alpha;
    coefs.a[1] = -2 * tcos;
    coefs.a[2] = 1 - alpha;

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsBandStop(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept
{
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float alpha = qAsBandWidth ? (tsin * std::sinh(std::log(2.0f) / 2.0f * q * omega / tsin)) : (tsin / (2.0f * q));

    Internal::Coefficients coefs {};
    coefs.b[0] = 1;
    coefs.b[1] = -2 * tcos;
    coefs.b[2] = 1;
    coefs.a[0] = 1 + alpha;
    coefs.a[1] = -2 * tcos;
    coefs.a[2] = 1 - alpha;

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsPeak(const float sampleRate, const float freq, const float gain, const float q, const bool qAsBandWidth) noexcept
{
    const float amp = std::pow(10.0f, gain / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float alpha = qAsBandWidth ? (tsin * std::sinh(std::log(2.0f) / 2.0f * q * omega / tsin)) : (tsin / (2.0f * q));

    Internal::Coefficients coefs {};
    coefs.b[0] = 1 + (alpha * amp);
    coefs.b[1] = -2 * tcos;
    coefs.b[2] = 1 - (alpha * amp);
    coefs.a[0] = 1 + (alpha /amp);
    coefs.a[1] = -2 * tcos;
    coefs.a[2] = 1 - (alpha /amp);

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsLowShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept
{
    const float amp = std::pow(10.0f, gain / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float beta = std::sqrt(amp) / q;

    Internal::Coefficients coefs {};
    coefs.b[0] = amp * ((amp + 1) - (amp - 1) * tcos + beta * tsin);
    coefs.b[1] = 2 * amp * ((amp - 1) - (amp + 1) * tcos);
    coefs.b[2] = amp * ((amp + 1) - (amp - 1) * tcos - beta * tsin);
    coefs.a[0] = (amp + 1) + (amp - 1) * tcos + beta * tsin;
    coefs.a[1] = -2 * ((amp - 1) + (amp + 1) * tcos);
    coefs.a[2] = (amp + 1) + (amp - 1) * tcos - beta * tsin;

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}

inline Audio::DSP::Biquad::Internal::Coefficients Audio::DSP::Biquad::Internal::GenerateCoefficientsHighShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept
{
    const float amp = std::pow(10.0f, gain / 40.0f);
    const float omega = 2.0f * static_cast<float>(M_PI) * freq / sampleRate;
    const float tsin = std::sin(omega);
    const float tcos = std::cos(omega);
    const float beta = std::sqrt(amp) / q;

    Internal::Coefficients coefs {};
    coefs.b[0] = amp * ((amp + 1) + (amp - 1) * tcos + beta * tsin);
    coefs.b[1] = -2 * amp * ((amp - 1) + (amp + 1) * tcos);
    coefs.b[2] = amp * ((amp + 1) + (amp - 1) * tcos - beta * tsin);
    coefs.a[0] = (amp + 1) - (amp - 1) * tcos + beta * tsin;
    coefs.a[1] = 2 * ((amp - 1) - (amp + 1) * tcos);
    coefs.a[2] = (amp + 1) - (amp - 1) * tcos - beta * tsin;

    coefs.b[0] = coefs.b[0] / coefs.a[0];
    coefs.b[1] = coefs.b[1] / coefs.a[0];
    coefs.b[2] = coefs.b[2] / coefs.a[0];
    coefs.a[1] = coefs.a[1] / coefs.a[0];
    coefs.a[2] = coefs.a[2] / coefs.a[0];

    return coefs;
}
