/**
 * @file Window.ipp
 * @brief Window methods implementation
 *
 * @author Pierre V
 * @date 2021-04-22
 */

template<Audio::DSP::Filter::WindowType Window>
inline void Audio::DSP::Filter::GenerateWindow(float *window, const std::size_t size, const bool symetric) noexcept
{
    UNUSED(size);
    UNUSED(symetric);
    switch (Window) {
    case WindowType::Hanning:
        return GenerateHanning(window, size, symetric);
    case WindowType::Hamming:
        return GenerateHamming(window, size, symetric);
    default:
        return GenerateHanning(window, size, symetric);
    }
}


inline void Audio::DSP::Filter::GenerateWindow(const WindowType type, const std::size_t size, float *window, const bool symetric) noexcept
{
    switch (type) {
    case WindowType::Hanning:
        return GenerateWindow<WindowType::Hanning>(window, size, symetric);
    case WindowType::Hamming:
        return GenerateWindow<WindowType::Hamming>(window, size, symetric);
    default:
        return GenerateWindow<WindowType::Hanning>(window, size, symetric);
    }
}

inline void Audio::DSP::Filter::GenerateHanning(float *window, const std::size_t size, const bool symetric) noexcept
{
    if (symetric) {
        for (auto i = 0u; i < size; i++) {
            window[i] = Hanning(i, size);
        }
    } else {
    }
}

inline void Audio::DSP::Filter::GenerateHamming(float *window, const std::size_t size, const bool symetric) noexcept
{
    if (symetric) {
        for (auto i = 0u; i < size; i++) {
            window[i] = Hamming(i, size);
        }
    } else {
    }
}
