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

    // for (auto i = 0u; i < 12; ++i) {
    //     SampleManager<Type>::WriteSampleFile("sample_" + std::to_string(i) + ".wav", _buffers[i]);
    // }
}

inline void Audio::Sampler::onAudioParametersChanged(void)
{
    for (auto &buffer : _buffers) {
        const auto newSize = GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize;
        buffer.resize(newSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    }
}

inline void Audio::Sampler::setExternalPaths(const ExternalPaths &paths)
{
    _externalPaths = paths;
    if (!paths.empty()) {
        loadSample<float>(paths[0]);
    }
}

inline void Audio::Sampler::sendNotes(const NoteEvents &notes)
{
    // std::cout << "\t - " << "_NOTES " << notes.size() << std::endl;
    _noteManager.feedNotes(notes);
}

inline void Audio::Sampler::receiveAudio(BufferView output)
{
    const auto outSize = output.size<float>();
    float *out = reinterpret_cast<float *>(output.byteData());

    float *sampleBuffer = nullptr;
    std::size_t sampleSize = 0u;

    // std::cout << "Receive audio" << std::endl;
    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        // if (!_noteManager.trigger(key))
        //     continue;
        const std::int32_t realKeyIdx = static_cast<std::int32_t>(key) % KeysPerOctave;
        const std::int32_t realOctave = static_cast<std::int32_t>(key) / KeysPerOctave;
        std::int32_t bufferKeyIdx = realKeyIdx - OctaveRootKey + 1;
        std::int32_t bufferOctave;

        if (bufferKeyIdx < 0) {
            bufferOctave = realOctave - RootOctave - 1;
            bufferKeyIdx += KeysPerOctave;
        } else
            bufferOctave = realOctave - RootOctave;

        sampleBuffer = _buffers[bufferKeyIdx].data<float>();
        sampleSize = _buffers[bufferKeyIdx].size<float>();

        if (!bufferOctave) {
            const auto baseIndex = _noteManager.readIndex(key);
            const int nextReadIndex = sampleSize - (baseIndex + audioSpecs().processBlockSize);
            const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;

            for (auto i = 0u; i < readSize; ++i) {
                const auto idx = baseIndex + i;
                out[i] += sampleBuffer[idx] * getEnveloppeGain(key, idx, _noteManager.trigger(key));
            }
            _noteManager.incrementReadIndex(key, sampleSize, readSize);
        }
        else {
            sampleBuffer = _buffers[bufferKeyIdx].data<float>();
            sampleSize = _buffers[bufferKeyIdx].size<float>();
            const std::size_t resampleSize = sampleSize * std::pow(2, -bufferOctave);
            const std::size_t resampleOffset = _noteManager.readIndex(key) * std::pow(2, bufferOctave);
            const int nextReadIndex = resampleSize - (_noteManager.readIndex(key) + audioSpecs().processBlockSize);
            const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;
            const auto resampleReadSize = readSize * std::pow(2, bufferOctave);

            DSP::Resampler<float>().resampleOctave<true, 8u>(sampleBuffer, out, resampleReadSize, audioSpecs().sampleRate, bufferOctave, resampleOffset);

            // Apply enveloppe
            for (auto i = 0u; i < readSize; ++i) {
                const auto idx = _noteManager.readIndex(key) + i;
                out[i] *= getEnveloppeGain(key, idx, _noteManager.trigger(key));
            }
            _noteManager.incrementReadIndex(key, resampleSize, readSize);
        }
    }
}

inline void Audio::Sampler::onAudioGenerationStarted(const BeatRange &range)
{
    _noteManager.reset();
}
