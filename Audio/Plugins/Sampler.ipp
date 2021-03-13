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

#include <iostream>

template<typename T>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    SampleSpecs specs;
    _buffers[0] = SampleManager<T>::LoadSampleFile(path, specs);

    // std::cout << "SIZE: " << buf.size<T>() << std::endl;
    // auto a = DSP::Resampler<T>::ResampleBySemitone(BufferView(buf), -11);

    // for (auto i = 0u; i < 11; ++i) {
        // _buffers.push(DSP::Resampler::Decimate<float>(v, static_cast<Semitone>(i)));
    // }

}

inline void Audio::Sampler::onAudioParametersChanged(void)
{
    // Buffer sourceBuffer = _buffers[0];

    for (auto &buffer : _buffers) {
        const auto newSize = GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize;
        buffer.resize(newSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    }

}

inline void Audio::Sampler::sendNotes(const NoteEvents &notes) noexcept
{
    std::cout << "_NOTES " << notes.size() << std::endl;
    std::cout << static_cast<int>(notes[0].type) << std::endl;
    _noteManager.feedNotes(notes);
}

inline void Audio::Sampler::receiveAudio(BufferView output) noexcept
{
    // std::cout << "AUDIO\n";
    // std::cout << "receiveAudio:::: " << _noteManager.getActiveNoteNumber() << std::endl;

    const auto outSize = output.size<float>();
    // const auto outSize = output.size<std::uint8_t>();
    const auto outChannel = 0u; //static_cast<std::uint32_t>(output.channelArrangement());

    // std::cout << "Sampler::output.byteData(): " << outSize << " " << reinterpret_cast<const int *>(output.byteData()) << std::endl;
    float *out = reinterpret_cast<float *>(output.byteData());
    // std::uint8_t *out = output.byteData();

    auto sampleSize = _buffers[0].size<float>();
    float *sampleBuffer = reinterpret_cast<float *>(_buffers[0].byteData());
    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        std::cout << "note: " << iKey << std::endl;
        const auto key = activeNote[iKey];
        if (!iKey) {
            for (auto i = 0u; i < outSize; ++i) {
                out[i] = _outputGain * sampleBuffer[_noteManager.readIndex(key)];
                _noteManager.incrementReadIndex(key, sampleSize);
            }
        } else {
            for (auto i = 0u; i < outSize; ++i) {
                out[i] += _outputGain * sampleBuffer[_noteManager.readIndex(key)];
                _noteManager.incrementReadIndex(key, sampleSize);
            }
        }
        break;
    }
    for (auto key : _noteManager.getActiveNoteBlock()) {
        std::cout << "note block:" << std::endl;
        // for (auto i = 0u; i < outSize; ++i) {
        //     output.data<float>(static_cast<Channel>(iChannel))[i] += _outputGain * _buffers[0].data<float>(static_cast<Channel>(iChannel))[_readIndex[key]];
        //     incrementReadIndex(key);
        // }
    }

    // std::cout << "sampler index: " << _readIndex[69] << std::endl;

    _noteManager.resetBlockCache();
}
