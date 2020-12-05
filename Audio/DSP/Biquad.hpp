/**
 * @ Author: Pierre Veysseyre
 * @ Description: Biquad.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <cstdint>
#include <numeric>

#include <array>

#include <iostream>

#include "immintrin.h"

namespace DSP
{
    // http://reanimator-web.appspot.com/articles/simdiir

    struct BiquadParam {
        struct Coefficients
        {
            float a[3] { 0.0 };
            float b[3] { 0.0 };
        };
        static_assert(sizeof(Coefficients) == 24, "Coefficients must take 24 bytes !");

        enum class Optimization : uint8_t {
            Classic, Optimized
        };

        enum class InternalForm : uint8_t {
            Direct1,        // 4 registers, 3 addOp -> better for fixed-points
            Direct2,        // 2 registers, 4 addOp -> better for fixed-points
            Transposed1,    // 4 registers, 4 addOp -> better for floating-points
            Transposed2     // 2 registers, 3 addOp -> better for floating-points
        };

        enum class FilterType : uint8_t {
            LowPass,
            HighPass,
            BandPass,
            BandPass2,
            BandStop, /* Notch */
            Peak,
            LowShelf,
            HighShelf
        };

        template<BiquadParam::FilterType Filter>
        [[nodiscard]] static Coefficients GenerateCoefficients(const double sampleRate, const double freq, const double gain, const double q, bool qAsBandWidth) noexcept;
    };

    struct alignas(16) Vec4 {

        Vec4 &operator+=(const Vec4 &other) noexcept {
            #pragma omp simd
            for (auto i = 0; i < 4; ++i)
                data[i] += other.data[i];
            return *this;
        }
        Vec4 &operator*=(const Vec4 &other) noexcept {
            #pragma omp simd
            for (auto i = 0; i < 4; ++i)
                data[i] *= other.data[i];
            return *this;
        }
        Vec4 operator*(const Vec4 &other) const noexcept {
            Vec4 res(*this);
            for (auto i = 0; i < 4; ++i)
                res.data[i] *= other.data[i];
            return res;
        }

        void print(void) const noexcept {
            std::cout << "[";
            for (auto i = 0; i < 4; ++i)
                std::cout << data[i] << ((i + 1) % 4 ? "," : "]");
            std::cout << std::endl;
        }
        float data[4];
    };
    struct alignas(16) Mat4 {

        Vec4 multVect(const Vec4 &vector) noexcept {
            Vec4 out { 0, 0, 0, 0 };

            return out;
        }

        void print(void) const noexcept {
            for (auto i = 0; i < 4; ++i) {
                data[i].print();
            }
            std::cout << std::endl;
        }
        Vec4 data[4];
    };


    struct MatrixCoeff {
        MatrixCoeff(const float b0, const float b1, const float b2, const float a1, const float a2) {
            float coefsTmp[4][8] {
                { 0, 0, 0, b0, b1, b2, a1, a2 },
                { 0, 0, b0, b1, b2, a1, a2, 0 },
                { 0, b0, b1, b2, a1, a2, 0, 0 },
                { b0, b1, b2, a1, a2, 0, 0, 0 },
            };
            for (auto i = 0u; i < 8; ++i) {
                coefsTmp[1][i] += a1 * coefsTmp[0][i];
                coefsTmp[2][i] += a1 * coefsTmp[1][i] + a2 * coefsTmp[0][i];
                coefsTmp[3][i] += a1 * coefsTmp[2][i] + a2 * coefsTmp[0][i];
            }
            // Transpose
            for (auto i = 0u; i < 8; ++i) {
                for (auto j = 0u; j < 4; ++j) {
                    _coefs[i].data[j] = coefsTmp[j][i];
                }
            }
        }

        // void process(const float *input, float *output, const std::uint32_t size) noexcept {
        //     Vec4 xp1 { _regInput[0] };
        //     Vec4 xp2 { _regInput[1] };
        //     Vec4 yp1 { _regOuput[0] };
        //     Vec4 yp2 { _regOuput[1] };

        //     for (auto i = 0; i < size; i +=4) {
        //         const Vec4 x0 { input[i] };
        //         const Vec4 x1 { input[i + 1] };
        //         const Vec4 x2 { input[i + 2] };
        //         const Vec4 x3 { input[i + 3] };


        //         Vec4 outY { 0, 0, 0, 0 };
        //         outY += _coefs[0] * x3;
        //         outY += _coefs[1] * x2;
        //         outY += _coefs[2] * x1;
        //         outY += _coefs[3] * x0;
        //         outY += _coefs[4] * xp1;
        //         outY += _coefs[5] * xp2;
        //         outY += _coefs[6] * yp1;
        //         outY += _coefs[7] * yp2;

        //         output[i] = outY.data[0];
        //         output[i + 1] = outY.data[1];
        //         output[i + 2] = outY.data[2];
        //         output[i + 3] = outY.data[3];

        //         xp1.data[0] = x3.data[0];
        //         xp2.data[0] = x2.data[0];
        //         yp1.data[0] = outY.data[3];
        //         yp2.data[0] = outY.data[2];

        //     }

        //     _regInput[0] = xp1.data[0];
        //     _regInput[1] = xp2.data[0];
        //     _regOuput[0] = yp1.data[0];
        //     _regOuput[1] = yp2.data[0];

        // }
        void processX(const float *input, float *output, const std::uint32_t size) noexcept {
            __m128 xp1 { _regInput[0] };
            __m128 xp2 { _regInput[1] };
            __m128 yp1 { _regOuput[0] };
            __m128 yp2 { _regOuput[1] };

            for (auto i = 0; i < size; i +=4) {
                __m128 x0 { input[i] };
                __m128 x1 { input[i + 1] };
                __m128 x2 { input[i + 2] };
                __m128 x3 { input[i + 3] };

                __m128 outY;

                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[0].data), x3);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[1].data), x2);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[2].data), x1);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[3].data), x0);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[4].data), xp1);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[5].data), xp2);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[6].data), yp1);
                outY += _mm_mul_ps(*reinterpret_cast<__m128 *>(&_coefs[7].data), yp2);

                _mm_store_ps(&output[i], outY);

                // _mm_storeh_pd
                // xp1.data[0] = x3.data[0];
                // xp2.data[0] = x2.data[0];
                // yp1.data[0] = outY.data[3];
                // yp2.data[0] = outY.data[2];

            }

            _mm_store_ps1(&_regInput[0], xp1);
            _mm_store_ps1(&_regInput[1], xp2);
            _mm_store_ps1(&_regOuput[0], yp1);
            _mm_store_ps1(&_regOuput[1], yp2);

        }

    private:
        Vec4 _coefs[8];
        float _regInput[2] { 0, 0 };
        float _regOuput[2] { 0, 0 };

    };




    template<BiquadParam::Optimization Opti, typename T>
    struct BiquadMaker;

    template<BiquadParam::InternalForm Form>
    class Biquad;

};

template<DSP::BiquadParam::InternalForm Form>
class DSP::Biquad
{
public:
    static constexpr auto Type = Form;
    static double SR;

    /** @brief Default constructor */
    Biquad(const double sampleRate, const double freq, const double gain, const double q, bool qAsBandWidth) {
        setup<BiquadParam::FilterType::LowPass>(sampleRate, freq, gain, q, qAsBandWidth);
    }

    /** @brief Setup the biquad according to the filter type and characteristics */
    template<BiquadParam::FilterType Filter>
    void setup(const double sampleRate, const double freq, const double gain, const double q, bool qAsBandWidth) noexcept {
        _coefs = BiquadParam::GenerateCoefficients<Filter>(sampleRate, freq, gain, q, qAsBandWidth);
        SR = sampleRate;
    }

    /** Process a block of samples */
    void processBlock(float *block, std::size_t len) noexcept;
    void processBlock1(float *block, std::size_t len) noexcept;

    /** Get the internal biquad coefficients */
    [[nodiscard]] const BiquadParam::Coefficients &coefficients(void) const noexcept { return _coefs; }
    // [[nodiscard]] BiquadParam::Coefficients &coefficients(void) noexcept { return _coefs; }


    float foo1(const float a, const float b, const float c) noexcept {
        return a * b + c;
    }
    float foo2(const float a, const float b, const float c) noexcept {
        return std::fma(a, b, c);
    }

// protected:
    BiquadParam::Coefficients   _coefs;
    float                      _regs[(Form == BiquadParam::InternalForm::Direct1 || Form == BiquadParam::InternalForm::Transposed1) ? 4 : 2] { 0.0 };

    /** @brief Process a sample into the biquad */
    [[nodiscard]] float process(const float in) noexcept;
    [[nodiscard]] float process1(const float in) noexcept;
};


template<DSP::BiquadParam::Optimization Opti, typename T>
struct DSP::BiquadMaker
{
    [[nodiscard]] static auto MakeBiquad(const double sampleRate, const double freq, const double gain = 3.0, const double q = 0.707, bool qAsBandWidth = false) noexcept {
        if constexpr (std::is_floating_point_v<T>) {
            if constexpr (Opti == DSP::BiquadParam::Optimization::Classic)
                return Biquad<BiquadParam::InternalForm::Transposed1>(sampleRate, freq, gain, q, qAsBandWidth);
            else if constexpr (Opti == BiquadParam::Optimization::Optimized)
                return Biquad<BiquadParam::InternalForm::Transposed2>(sampleRate, freq, gain, q, qAsBandWidth);
        } else if constexpr (std::is_integral<T>()) {
            if constexpr (Opti == BiquadParam::Optimization::Classic)
                return Biquad<BiquadParam::InternalForm::Direct1>(sampleRate, freq, gain, q, qAsBandWidth);
            else if constexpr (Opti == BiquadParam::Optimization::Optimized)
                return Biquad<BiquadParam::InternalForm::Direct2>(sampleRate, freq, gain, q, qAsBandWidth);
        }
    }
};

#include "Biquad.ipp"

// // static_assert(alignof(DSP::Biquad) == 8, "Biquad must be aligned to 8 bytes !");
// // static_assert(sizeof(DSP::Biquad) == 80, "Biquad must take 80 bytes !");
