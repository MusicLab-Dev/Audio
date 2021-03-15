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


template<typename Type>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    SampleSpecs specs;
    _buffers[OctaveRootKey] = SampleManager<Type>::LoadSampleFile(path, specs);

    auto rootSize = _buffers[OctaveRootKey].size<Type>();
    BufferView view(_buffers[OctaveRootKey]);
    for (auto i = 0u; i < OctaveRootKey; ++i) {
        auto bufferSize = DSP::Resampler<Type>::GetResamplingSizeSemitone(rootSize, -1) * sizeof(Type);
        std::cout << "loadSample::bufferSize: " << bufferSize / sizeof(Type) << std::endl;
        _buffers[i].resize(bufferSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);

        DSP::Resampler<Type>::ResampleSemitone(
            view.data<Type>(),
            _buffers[i].data<Type>(),
            rootSize,
            -1
        );

        // _buffers[i].resize(rootSize * 4.0 * sizeof(Type), audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
        // DSP::Resampler<float>::Internal::InterpolateOctave(
        //     view.data<Type>(),
        //     _buffers[i].data<Type>(),
        //     rootSize,
        //     2
        // );

        // rootSize = bufferSize;
        // view = _buffers[i];
        break;
    }
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

#include <Audio/DSP/Biquad.hpp>

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

    auto idx = 0;

    auto sampleSize = _buffers[idx].size<float>();
    std::cout << "Sampler::receiveAudio::sampleSize: " << sampleSize << std::endl;
    float *sampleBuffer = _buffers[idx].data<float>();

    bool hasProcess = false;

    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        // if (!_noteManager.trigger(key))
        //     continue;
        hasProcess = true;
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

    // std::cout << "sampler index: " << _noteManager.readIndex(69) << std::endl;

    static int a = 0;
    static auto filter = DSP::BiquadMaker<DSP::BiquadParam::Optimization::Optimized>::MakeBiquad<float>();
    filter.setupCoefficients(DSP::BiquadParam::GenerateCoefficients<
        DSP::BiquadParam::FilterType::LowPass
        >(44100, (a), 0, 1.707, false));

    // if (hasProcess)
    // filter.processBlock(out, outSize);
    // else
    //     filter.resetRegisters();
    // _noteManager.resetBlockCache();

    a += 10;
    if (a >= 44100 / 2)
        a = 0;
}
