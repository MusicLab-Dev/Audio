/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/BufferOctave.hpp>
#include <Audio/SampleFile/SampleManager.hpp>

template<typename Type>
inline void Audio::Sampler::loadSample(const std::string_view &path)
{
    SampleSpecs specs;
    _buffers[OctaveRootKey] = SampleManager<Type>::LoadSampleFile(std::string(path), specs);
    GenerateOctave<Type>(_buffers[OctaveRootKey], _buffers);
}

inline void Audio::Sampler::onAudioParametersChanged(void)
{
    // Buffer sourceBuffer = _buffers[0];

    for (auto &buffer : _buffers) {
        const auto newSize = GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize;
        buffer.resize(newSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    }
    // _enveloppe.setSampleRate(_specs.sampleRate());
}

inline void Audio::Sampler::setExternalPaths(const ExternalPaths &paths)
{
    if (!paths.empty()) {
        loadSample<float>(paths[0]);
    }
}

inline void Audio::Sampler::sendNotes(const NoteEvents &notes)
{
    // std::cout << "\t - " << "_NOTES " << notes.size() << std::endl;
    _noteManager.feedNotes(notes);
}

#include <Audio/DSP/Biquad.hpp>

inline void Audio::Sampler::receiveAudio(BufferView output)
{
    // std::cout << "AUDIO: " << _controls.size() << std::endl;
    // std::cout << "receiveAudio:::: " << _noteManager.getActiveNoteNumber() << std::endl;

    const auto outSize = output.size<float>();
    float *out = reinterpret_cast<float *>(output.byteData());


    float *sampleBuffer = nullptr;
    std::size_t sampleSize = 0u;

    // std::cout << "Sampler::receiveAudio::sampleSize: " << sampleSize << std::endl;

    // bool hasProcess = false;

    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        // if (!_noteManager.trigger(key))
        //     continue;
        // const std::size_t bufferIdx = OctaveRootKey - RootKey + key;
        const std::size_t bufferIdx = OctaveRootKey;
        sampleBuffer = _buffers[bufferIdx].data<float>();
        sampleSize = _buffers[bufferIdx].size<float>();
        // hasProcess = true;
        const int nextReadIndex = sampleSize - (_noteManager.readIndex(key) + audioSpecs().processBlockSize);
        const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;
        // std::cout << "in: " << _noteManager.readIndex(key) << std::endl;
        // std::cout << "sampleSize: " << sampleSize << std::endl;
        // std::cout << "nextReadIndex: " << nextReadIndex << std::endl;
        // std::cout << "readSize: " << readSize << std::endl;
        // std::cout << std::endl;
        for (auto i = 0u; i < readSize; ++i) {
            const auto idx = _noteManager.readIndex(key) + i;
            out[i] += sampleBuffer[idx] * _noteManager.getEnveloppeGain(key, idx, _noteManager.trigger(key))
                / static_cast<float>(activeNote);
            // _noteManager.incrementReadIndex(key, sampleSize);
        }
        _noteManager.incrementReadIndex(key, sampleSize, readSize);
    }
    // if (hasProcess)
    //     std::cout << " - HAS PROCESS: " << hasProcess << std::endl;
}
