/**
 * @file Window.ipp
 * @brief Window methods implementation
 *
 * @author Pierre V
 * @date 2021-04-22
 */

inline void Audio::DSP::Filter::GenerateWindow(const WindowType type, const std::size_t size, float *window, const bool isSymetric) noexcept
{
    switch (type) {
    case WindowType::Hanning:
        return Hanning(size, window, isSymetric);
    case WindowType::Hamming:
        return Hamming(size, window, isSymetric);
    default:
        return Hanning(size, window, isSymetric);
    }
}

inline void Audio::DSP::Filter::Hanning(const std::size_t size, float *outputWindow, const bool isSymetric) noexcept
{
    if (isSymetric) {
        for (auto i = 0u; i < size; i++) {
            outputWindow[i] = 0.5f - 0.5f * (std::cos(2.0 * M_PI * i / static_cast<float>(size - 1)));
        }
    } else {
    }
}

inline void Audio::DSP::Filter::Hamming(const std::size_t size, float *outputWindow, const bool isSymetric) noexcept
{
    if (isSymetric) {
        for (auto i = 0u; i < size; i++) {
            outputWindow[i] = 0.54f - 0.46f * (std::cos(2.0 * M_PI * i / static_cast<float>(size - 1)));
        }
    } else {
    }
}