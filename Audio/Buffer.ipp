/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer implementation
 */

inline void Audio::Internal::BufferBase::swap(Internal::BufferBase &other) noexcept
{
    std::swap(_data, other._data); std::swap(_channelByteSize, other._channelByteSize);
    std::uint32_t tmp = other._sampleRate;
    other._sampleRate = tmp;
    _sampleRate = tmp;

    tmp = other._arrangement;
    other._arrangement = _arrangement;
    _arrangement = tmp;
}
