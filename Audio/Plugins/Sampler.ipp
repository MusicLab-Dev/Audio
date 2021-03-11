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

inline void Audio::Sampler::incrementReadIndex(Key key, std::size_t amount) noexcept
{
    if (_readIndex[key] + amount >= _buffers[0].size<float>()) {
        _readIndex[key] = 0u;
    } else {
        _readIndex[key] += amount;
    }
}

#include <iostream>

template<typename T>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    SampleSpecs specs;
    _buffers[0] = SampleManager<T>::LoadSampleFile(path, specs);

    // std::cout << "SIZE: " << buf.size<T>() << std::endl;
    // auto a = DSP::Resampler<T>::ResampleBySemitone(BufferView(buf), -11);

    for (auto i = 0u; i < 11; ++i) {
        // _buffers.push(DSP::Resampler::Decimate<float>(v, static_cast<Semitone>(i)));
    }

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
    std::cout << "_NOTES\n" ;
    std::cout << static_cast<int>(notes[0].type) << std::endl;
    _noteManager.feedNotes(notes);
}

inline void Audio::Sampler::receiveAudio(BufferView output) noexcept
{
    // std::cout << "AUDIO\n";
    // std::cout << "receiveAudio:::: " << _noteManager.getActiveNoteNumber() << std::endl;

    // const auto outSize = output.size<float>();
    const auto outSize = output.size<std::uint8_t>();
    const auto outChannel = static_cast<std::uint32_t>(output.channelArrangement());
    // const auto noteNumber = _noteManager.getActiveNoteNumber();
    // const auto noteBlockNumber = _noteManager.getActiveNoteNumberBlock();

    static std::size_t cpt = 0u;

    std::cout << "Sampler::output.byteData(): " << outSize << " " << reinterpret_cast<const int *>(output.byteData()) << std::endl;
    // float *out = reinterpret_cast<float *>(output.byteData());
    std::uint8_t *out = output.byteData();

    // for (auto i = 0u; i < outSize; ++i) {
    //     out[i] = std::sin(2 * 3.14 * 440 * cpt / 44100);
    //     ++cpt;
    //     if (cpt > 1024)
    //         cpt = 0;
    // }

    for (auto key : _noteManager.getActiveNote()) {
        // std::cout << "note:" << std::endl;
        for (auto iChannel = 0u; iChannel < outChannel; ++iChannel) {
            for (auto i = 0u; i < outSize; ++i) {
                output.data<float>(static_cast<Channel>(iChannel))[i] += _outputGain * _buffers[0].data<float>(static_cast<Channel>(iChannel))[_readIndex[key]];
                incrementReadIndex(key);
            }
        }
    }
    for (auto key : _noteManager.getActiveNoteBlock()) {
        std::cout << "note block:" << std::endl;
        for (auto iChannel = 0u; iChannel < outChannel; ++iChannel) {
            for (auto i = 0u; i < outSize; ++i) {
                output.data<float>(static_cast<Channel>(iChannel))[i] += _outputGain * _buffers[0].data<float>(static_cast<Channel>(iChannel))[_readIndex[key]];
                incrementReadIndex(key);
            }
        }
    }

    // std::cout << "sampler index: " << _readIndex[69] << std::endl;

    _noteManager.resetBlockCache();
}
