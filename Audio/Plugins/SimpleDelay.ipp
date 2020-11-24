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
    _cache[_writeIdx] = BufferViews(inputs);
    incrementIdx(_writeIdx);
}

inline void Audio::SimpleDelay::receiveAudio(BufferView output) noexcept
{
    // need to merge the buffers in _cache[_readIdx]
    output = _cache[_readIdx][0];
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
