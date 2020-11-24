/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */

inline Audio::IPlugin::Flags Audio::Oscillator::getFlags(void) const noexcept
{
    return static_cast<Flags>(
        static_cast<std::size_t>(Flags::AudioOutput) |
        static_cast<std::size_t>(Flags::ControlInput) |
        static_cast<std::size_t>(Flags::NoteInput)
    );
}

inline void Audio::Oscillator::sendAudio(const BufferViews &inputs) noexcept
{

}

inline void Audio::Oscillator::receiveAudio(BufferView output) noexcept
{

}

inline void Audio::Oscillator::sendNotes(const NoteEvents &notes) noexcept
{
    _noteManager->processNotes(notes);
}

inline void Audio::Oscillator::receiveNotes(NoteEvents &notes) noexcept
{

}


inline void Audio::Oscillator::sendControls(const Controls &controls) noexcept
{

}
