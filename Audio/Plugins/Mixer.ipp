/**
 * @ Author: Pierre Veysseyre
 * @ Description: Mixer implementation
 */

inline Audio::IPlugin::Flags Audio::Mixer::getFlags(void) const noexcept
{
    return static_cast<Flags>(
        static_cast<std::size_t>(Flags::AudioInput) |
        static_cast<std::size_t>(Flags::AudioOutput) |
        static_cast<std::size_t>(Flags::NoteInput) |
        static_cast<std::size_t>(Flags::NoteOutput) |
        static_cast<std::size_t>(Flags::ControlInput)
    );
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs) noexcept
{
}

inline void Audio::Mixer::receiveAudio(BufferView output) noexcept
{
}


inline void Audio::Mixer::onAudioGenerationStarted(const BeatRange &range) noexcept
{

}
