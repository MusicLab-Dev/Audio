/**
 * @ Author: Pierre Veysseyre
 * @ Description: SimpleDelay implementation
 */

inline Audio::IPlugin::Flags Audio::SimpleDelay::getFlags(void) const noexcept
{
    return static_cast<Flags>(
        static_cast<std::size_t>(Flags::AudioInput) |
        static_cast<std::size_t>(Flags::AudioOutput) |
        static_cast<std::size_t>(Flags::ControlInput)
    );
}

inline void Audio::SimpleDelay::sendAudio(const BufferViews &inputs) noexcept
{
    const auto byteSize = _cache[0].channelByteSize() * static_cast<std::uint32_t>(_cache[0].channelArrangement());

    // std::memcpy(_cache[_readIdx].byteData(), inputs[0].byteData(), byteSize);
    incrementIdx(_writeIdx);
}

inline void Audio::SimpleDelay::receiveAudio(BufferView output) noexcept
{
    // need to merge the buffers in _cache[_readIdx]
    output = _cache[_readIdx];
    incrementIdx(_readIdx);
}


inline void Audio::SimpleDelay::onAudioGenerationStarted(const BeatRange &range) noexcept
{

}

inline void Audio::SimpleDelay::incrementIdx(Index &index) noexcept
{
    if (++index >= static_cast<Index>(_delay * _cache.size()))
        index = 0u;
}
