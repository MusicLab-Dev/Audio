/**
 * @ Author: Pierre Veysseyre
 * @ Description: Filter.hpp
 */

#include "Window.hpp"

struct Filtering
{
    static void FilterX(const Audio::DSP::FilterSpecs specs, const float *input, float *output, const std::size_t inputSize, const bool normal = false) noexcept
    {
        const auto windowSize = specs.windowSize;
        const auto halfWindowSize = windowSize / 2;
        std::vector<float> windowFilter(windowSize);
        Audio::DSP::WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
        Audio::DSP::FIR::DesignFilter(specs, windowFilter.data(), windowSize);

        float * const windowData = windowFilter.data();

        /**
         * @todo Woo 'group delay' my d***k !
         *  2. Si le buffer est null, resize le avec des 0 pour la première frame
         * 3. Sinon utilise le pour générer le début de la frame
         * 4. Stoque halfwindowsize dans le buffer pour la frame suivante
         * 5. Enjoy """"""""""""""free"""""""""""""" money bitch
         */

        { // Process the first frame
            auto totalProcess = 1;
            for (auto i = 0u; i < windowSize; ++i) {
                float filteredSample = 0.0f;
                for (auto j = 0u; j < totalProcess; ++j) {
                    const auto inputIndex = i - j;
                    const auto windowIndex = j;
                    filteredSample += input[inputIndex] * windowData[windowIndex];
                }
                ++totalProcess;
                if (i >= halfWindowSize)
                    output[i - halfWindowSize] = filteredSample;
            }
        }

        { // Process the middle frames
            const auto end = inputSize;
            for (auto i = windowSize; i < end; ++i) {
                float filteredSample = 0.0f;
                for (auto j = 0u; j < windowSize; ++j) {
                    const auto inputIndex = i - j;
                    const auto windowIndex = j;
                    filteredSample += input[inputIndex] * windowData[windowIndex];
                }
                output[i - halfWindowSize] = filteredSample;
            }
        }


        { // Process the last frame
            const auto inputSizeMinusHalfWindowSize = inputSize - halfWindowSize;
            auto totalProcess = windowSize;
            for (auto i = 0u; i < halfWindowSize; ++i) {
                float filteredSample = 0.0f;
                for (auto j = 0u; j < totalProcess; ++j) {
                    const auto inputIndex = inputSize - windowSize + j + i + 1;
                    const auto windowIndex = j;
                    filteredSample += input[inputIndex] * windowData[windowIndex];
                }
                output[inputSize - windowSize + i] = filteredSample;
            }
        }


        return;

        // for (auto i = inputSize / 2; i < inputSize; ++i)
        //     const_cast<float *>(input)[i] = 0.f;

        // for (auto i = 0u; i < inputSize / 2; ++i) {
        //     const auto idx = (inputSize / 2) - i - 1u;
        //     const auto dt = (idx < windowSize ? windowSize - 1 - idx : 0u);
        //     const auto inputOffset = idx > windowSize - 1 ? idx - windowSize + 1 : 0;
        //     // std::cout << i << ", " << idx << "," << dt << ", " << inputOffset << std::endl;
        //     output[idx] = FilterXImpl (input + inputOffset, windowFilter.data(), windowSize, dt);
        // }
        // return;

        // { // Process the second frame
        //     for (auto i = windowSize; i < windowSize * 2; ++i) {
        //         float sample = 0.0f;
        //         for (auto j = 0u; j < windowSize; ++j) {
        //             const auto inputIndex = i - j;
        //             const auto windowIndex = j;
        //             sample += input[inputIndex] * windowData[windowIndex];
        //         }
        //         output[i] = sample;
        //     }
        // }
        // return;

    {
        for (auto i = 0u; i < inputSize - windowSize / 2; ++i) {
        }
        return;
    }


// input:     12345
// output:    12345
// input: 000012345
// win:       abcba

// 012210
// 012210

// readIn:  _____6
// exOut:   _____6
// reaOut:  __1236


















































        constexpr bool RemoveDelay = true;
        constexpr bool ProcessFirstChunk = true;





        // if (normal) {
            for (auto i = 0u; i < inputSize; ++i) {
                const auto idx = inputSize - i - 1u;
                const auto dt = (idx < windowSize ? windowSize - 1 - idx : 0u);
                const auto inputOffset = idx > windowSize - 1 ? idx - windowSize + 1 : 0;
                // std::cout << i << ", " << idx << "," << dt << ", " << inputOffset << std::endl;
                output[idx] = FilterXImpl (input + inputOffset, windowFilter.data(), windowSize, dt);
            }

            return;
        // }


        // std::cout << "INPUT: " << input << " -> " << input[inputSize - 1] << std::endl;
        // for (auto k = 0; k < 1; ++k) {
        //     const auto kk = inputSize - k - 1;
        //     std::cout << " -> " << (input - windowSize / 2)[k]
        //     << " -- " << (input - windowSize / 2)[kk] << std::endl;
        // }
        // return;

        auto cptFirst = 0u;
        auto cptBody = 0u;
        auto cptLast = 0u;
        // This remove delay due to the convolution
        if constexpr (RemoveDelay) {
            // Body chunk
            for (auto i = windowSize / 2; i < inputSize - windowSize / 2; ++i) {
                // const auto idx = inputSize - i - 1u;
                // const auto idx = i;
                // std::cout << "i: " << i << "  -> " << idx - windowSize / 2 << std::endl;
                // output[i] = FilterXImpl(input + i - windowSize / 2, windowFilter.data(), windowSize, 0u);
                ++cptBody;
                // return;
            }
            // First chunk
            if constexpr (ProcessFirstChunk) {
                // for (auto i = inputSize - windowSize + 1; i < inputSize - windowSize / 2; ++i) {
                for (auto i = 0u; i < windowSize / 2; ++i) {
                    // const auto idx = inputSize - i - 1u;
                    output[i] = FilterXImpl(input, windowFilter.data(), windowSize, windowSize / 2 - i);
                    ++cptFirst;
                }
                // Last chunk

                // auto outputIndex = inputSize - windowSize / 2;
                // auto inputIndex = outputIndex - windowSize;


                for (auto i = inputSize - windowSize / 2; i < inputSize; ++i) {
                    // if (cptLast > 3)
                    //     break;
                    // std::cout << "i= " << i << ", " << (i - (1 + windowSize / 2)) << std::endl;
                    // const auto idx = inputSize - i - 1u;
                    float out = 0.0f;

                    auto j = i - windowSize / 2;
                    for (auto k = windowSize; k > 0; --k) {
                        out += (input[j + k - 1] * windowFilter.data()[k - 1]);
                    }

                    // output[i] = out;//FilterXImpl(input + i - windowSize / 2 - 1, windowFilter.data(), windowSize, 0);
                    // output[i] = FilterXImpl(input + i - windowSize / 2 - 1, windowFilter.data(), windowSize, 0);
                    ++cptLast;
                }
            }
            std::cout << "CPT: " << cptFirst << ", " << cptBody << ", " << cptLast << " -> " << cptFirst + cptBody + cptLast << std::endl;


        } else {
        //     // Hihest part of the input
        //     for (auto i = 0u; i < inputSize - windowSize + 1; ++i) {
        //         const auto idx = inputSize - i - 1u;
        //         // std::cout << "i: " << i << ", idx: " << idx << ", zeroPad: " << 0 << ", inOffset" << (idx - windowSize + 1) << std::endl;
        //         output[idx] = FilterXImpl(input + idx - windowSize, windowFilter.data(), windowSize, 0u);
        //     }
        //     // Lower part of the input
        //     if constexpr (ProcessFirstChunk) {
        //         for (auto i = inputSize - windowSize + 1; i < inputSize; ++i) {
        //             const auto idx = inputSize - i - 1u;
        //             output[idx] = FilterXImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
        //         }
        //     }
        }

    }

    static void FilterLastInputX(const Audio::DSP::FilterSpecs specs, const float *input, float *output, const std::size_t inputSize, const float *lastInput, const std::size_t lastInputSize) noexcept
    {
        // const auto windowSize = specs.windowSize;
        // std::vector<float> windowFilter(windowSize);
        // Audio::DSP::WindowMaker::GenerateFilterCoefficients(specs.windowType, windowSize, windowFilter.data());
        // Audio::DSP::FIR::DesignFilter(specs, windowFilter.data(), windowSize);

        // constexpr bool RemoveDelay = true;
        // constexpr bool ProcessFirstChunk = true;

        // std::cout << "INPUT: " << std::endl;


        // auto cptA = 0u;
        // auto cptB = 0u;
        // if constexpr (RemoveDelay) {
        //     // Highest part of the input
        //     for (auto i = 0u; i < inputSize - windowSize / 2 + 1; ++i) {
        //         const auto idx = inputSize - i - 1u;
        //         output[idx] = FilterXImpl(input + idx - windowSize / 2, windowFilter.data(), windowSize, 0u);
        //         ++cptA;
        //     }
        //     // Lower part of the input
        //     if constexpr (ProcessFirstChunk) {
        //         const auto lastInputOffset = (lastInputSize < windowSize) ? 0u : (lastInputSize - (windowSize - 1) - 1);
        //         for (auto i = inputSize - windowSize + 1; i < inputSize - windowSize / 2; ++i) {
        //             const auto idx = inputSize - i - 1u;
        //             output[idx - windowSize / 2] = FilterXLastInputImpl(input, lastInput + lastInputOffset, windowFilter.data(), windowSize, windowSize - 1 - idx, lastInputSize - lastInputOffset);
        //             // output[idx - windowSize / 2] = FilterImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
        //             ++cptB;
        //         }
        //     }
        // } else {
        //     // Hihest part of the input
        //     for (auto i = 0u; i < inputSize - windowSize + 1; ++i) {
        //         const auto idx = inputSize - i - 1u;
        //         // std::cout << "i: " << i << ", idx: " << idx << ", zeroPad: " << 0 << ", inOffset" << (idx - windowSize + 1) << std::endl;
        //         output[idx] = FilterXImpl(input + idx - windowSize, windowFilter.data(), windowSize, 0u);
        //     }
        //     // Lower part of the input
        //     if constexpr (ProcessFirstChunk) {
        //         const auto lastInputOffset = (lastInputSize < windowSize) ? 0u : (lastInputSize - (windowSize - 1));
        //         for (auto i = inputSize - windowSize + 1; i < inputSize; ++i) {
        //             const auto idx = inputSize - i - 1u;
        //             output[idx] = FilterXLastInputImpl(input, lastInput, windowFilter.data(), windowSize, windowSize - 1 - idx, lastInputSize - lastInputOffset);
        //             // output[idx] = FilterImpl(input, windowFilter.data(), windowSize, windowSize - 1 - idx);
        //         }
        //     }
        // }

        // std::cout << "Processed " << cptA << ", " << cptB << " = " << cptA + cptB << std::endl;

    }



    static float FilterXImpl(
                const float *input, const float *window,
                const std::size_t processSize,
                const std::size_t zeroPadBegin, const std::size_t zeroPadEnd = 0u) noexcept
    {
        float sample { 0.0 };

        // Input convultion
        for (auto i = zeroPadBegin; i < processSize - zeroPadEnd; ++i) {
            const auto idx = processSize - i - 1;
            // std::cout << "   i -> "  << i << std::endl;
            // std::cout << "   idx -> "  << idx << std::endl;
            sample += (input[idx] * window[idx + zeroPadBegin]);
            // return sample;
        }
        return sample;
    }

    static float FilterXImpl2(
                const float *input, const float *window,
                const std::size_t processSize,
                const std::size_t zeroPadBegin, const std::size_t zeroPadEnd = 0u) noexcept
    {
        float sample { 0.0 };

        // Input convultion
        for (auto i = zeroPadBegin; i < processSize - zeroPadEnd; ++i) {
            const auto idx = processSize - i - 1;
            sample += (input[i] * window[i + zeroPadBegin]);
        }
        return sample;
    }

    static float FilterXLastInputImpl(
                const float *input, const float *lastInput,
                const float *window, const std::size_t processSize,
                const std::size_t zeroPad, const std::size_t zeroPadLastInput) noexcept
    {
        float sample { 0.0 };

        // Input convolution
        for (auto i = zeroPad; i < processSize; ++i) {
            const auto idx = processSize - i - 1;
            sample += (input[idx] * window[idx + zeroPad]);
        }

        // Last input convolution
        for (auto i = 0; i < zeroPad; ++i) {
            const auto idx = processSize - i - 1;
            sample += (lastInput[idx] * window[zeroPad - i - 1]);
        }
        return sample;
    }


};