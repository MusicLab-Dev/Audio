/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/DSP/Resampler.hpp>
#include <Audio/SampleFile/SampleManager.hpp>

inline Audio::IPlugin::Flags Audio::Sampler::getFlags(void) const noexcept
{
    return static_cast<Flags>(
        static_cast<std::size_t>(Flags::AudioOutput) |
        static_cast<std::size_t>(Flags::ControlInput) |
        static_cast<std::size_t>(Flags::NoteInput)
    );
}

inline void Audio::Sampler::incrementReadIndex(Key key, std::size_t size) noexcept
{
    if (_readIndex[key] + size >= _buffers[0].size<float>()) {
        _readIndex[key] = 0u;
    } else {
        _readIndex[key] += size;
    }
}

// #include <iostream>

template<typename T>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    SampleSpecs specs;
    Buffer sampleBuffer = SampleManager<T>::LoadSampleFile(path, specs);

    // std::cout << "SIZE: " << buf.size<T>() << std::endl;
    // auto a = DSP::Resampler<T>::ResampleBySemitone(BufferView(buf), -11);
    auto &buffer = _buffers.push(sampleBuffer);

    for (auto i = 0u; i < 11; ++i) {
        // _buffers.push(DSP::Resampler::Decimate<float>(v, static_cast<Semitone>(i)));
    }

}

inline void Audio::Sampler::sendNotes(const NoteEvents &notes) noexcept
{
    _noteManager.feedNotes(notes);
}

inline void Audio::Sampler::receiveAudio(BufferView output) noexcept
{
    // std::cout << "receiveAudio:::: " << _noteManager.getActiveNoteNumber() << std::endl;
    const auto outSize = output.size<float>();
    const auto outChannel = static_cast<std::uint32_t>(output.channelArrangement());
    const auto noteNumber = _noteManager.getActiveNoteNumber();
    const auto noteBlockNumber = _noteManager.getActiveNoteNumberBlock();

    for (auto key : _noteManager.getActiveNote()) {
        for (auto iChannel = 0u; iChannel < outChannel; ++iChannel) {
            for (auto i = 0u; i < outSize; ++i) {
                output.data<float>(static_cast<Channel>(iChannel))[i] = _outputGain * _buffers[0].data<float>(static_cast<Channel>(iChannel))[_readIndex[key]];
                incrementReadIndex(key);
            }
        }
    }

    _noteManager.resetBlockCache();
}
