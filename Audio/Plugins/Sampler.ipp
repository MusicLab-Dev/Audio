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

    // GenerateOctave<Type>(_buffers[OctaveRootKey], _buffers);
    // const auto inSize = _buffers[0].size<float>();
    // _tmp.resize(inSize / 2 * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);

    const auto size = _buffers[OctaveRootKey].size<Type>();
    for (int i = 0; i < 3; ++i) {
        _buffers[i].resize(size / std::pow(2, i + 1) * sizeof(Type), 44100, ChannelArrangement::Mono, Format::Floating32);
        DSP::Resampler<float>().resampleOctave<false, 8u>(_buffers[OctaveRootKey].data<Type>(), _buffers[i].data<Type>(), size, audioSpecs().sampleRate, i + 1);
        SampleManager<Type>::WriteSampleFile("sample_" + std::to_string(i + 1) + ".wav", _buffers[i]);
    }
    for (int i = 0; i < 3; ++i) {
        _buffers[i].resize(size * std::pow(2, i + 1) * sizeof(Type), 44100, ChannelArrangement::Mono, Format::Floating32);
        DSP::Resampler<float>().resampleOctave<false, 8u>(_buffers[OctaveRootKey].data<Type>(), _buffers[i].data<Type>(), size, audioSpecs().sampleRate, -i - 1);
        SampleManager<Type>::WriteSampleFile("sample_" + std::to_string(-i - 1) + ".wav", _buffers[i]);
    }

    // DSP::Resampler::ResampleOctave<false>(
    //     _buffers[0].data<float>(),
    //     _tmp.data<float>(),
    //     inSize,
    //     44100,
    //     1
    // );

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

inline void Audio::Sampler::receiveAudio(BufferView output)
{
    // std::cout << "AUDIO: " << _controls.size() << std::endl;
    // std::cout << "receiveAudio:::: " << _noteManager.getActiveNoteNumber() << std::endl;

    const auto outSize = output.size<float>();
    float *out = reinterpret_cast<float *>(output.byteData());


    float *sampleBuffer = nullptr;
    std::size_t sampleSize = 0u;

    // std::cout << "Sampler::receiveAudio::sampleSize: " << sampleSize << std::endl;

    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        // if (!_noteManager.trigger(key))
        //     continue;
        // const std::int32_t bufferIdxDt = static_cast<std::int32_t>(key) - static_cast<std::int32_t>(RootKey) + OctaveRootKey;
        // const std::size_t bufferKeyIdx = (bufferKeyIdxDt >= 0) ? bufferKeyIdxDt % KeysPerOctave : (KeysPerOctave + bufferKeyIdxDt) % KeysPerOctave;
        // const std::int32_t bufferOctave = (bufferKeyIdxDt >= 0) ? bufferKeyIdxDt / KeysPerOctave : -((KeysPerOctave - bufferKeyIdxDt - 1) / KeysPerOctave);

        const std::int32_t realKeyIdx = static_cast<std::int32_t>(key) % KeysPerOctave;
        const std::int32_t realOctave = static_cast<std::int32_t>(key) / KeysPerOctave;
        std::int32_t bufferKeyIdx = realKeyIdx - OctaveRootKey + 1;
        std::int32_t bufferOctave;

        if (bufferKeyIdx < 0) {
            bufferOctave = realOctave - RootOctave - 1;
            bufferKeyIdx += KeysPerOctave;
        } else
            bufferOctave = realOctave - RootOctave;

        // bufferKeyIdx = OctaveRootKey;

        // std::cout << "realKeyIdx " << realKeyIdx << " realOctave " << realOctave << " bufferKeyIdx " << bufferKeyIdx << " bufferOctave " << bufferOctave << std::endl;

        // std::cout << "buffer idx dt: " << bufferKeyIdxDt << std::endl;
        // std::cout << "buffer idx: " << bufferKeyIdx << std::endl;
        // std::cout << "bufferOctave: " << bufferOctave << std::endl;

        sampleBuffer = _buffers[bufferKeyIdx].data<float>();
        sampleSize = _buffers[bufferKeyIdx].size<float>();
        // sampleBuffer = _tmp.data<float>();
        // sampleSize = _tmp.size<float>();
        // return;

        // std::cout << "in: " << _noteManager.readIndex(key) << std::endl;
        // std::cout << "sampleSize: " << sampleSize << std::endl;
        // std::cout << "nextReadIndex: " << nextReadIndex << std::endl;
        // std::cout << "readSize: " << readSize << std::endl;
        // std::cout << std::endl;
        if (!bufferOctave) {
            const auto baseIndex = _noteManager.readIndex(key);
            const int nextReadIndex = sampleSize - (baseIndex + audioSpecs().processBlockSize);
            const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;

            // std::cout << "baseIndex: " << baseIndex << ", nextRead: " << nextReadIndex << ", readSize: " << readSize << std::endl;
            for (auto i = 0u; i < readSize; ++i) {
                const auto idx = baseIndex + i;
                out[i] += sampleBuffer[idx] * _noteManager.getEnveloppeGain(key, idx, _noteManager.trigger(key)) / static_cast<float>(activeNote);
            }
            _noteManager.incrementReadIndex(key, sampleSize, readSize);
        }
        else {
            sampleBuffer = _buffers[bufferKeyIdx].data<float>();
            sampleSize = _buffers[bufferKeyIdx].size<float>();
            // const std::size_t resampleSize = (bufferOctave > 0) ? sampleSize / 2 : sampleSize * 2;
            const std::size_t resampleSize = sampleSize * std::pow(2, -bufferOctave);
            // const std::size_t resampleOffset = (bufferOctave < 0) ? _noteManager.readIndex(key) / 2 : _noteManager.readIndex(key) * 2;
            const std::size_t resampleOffset = _noteManager.readIndex(key) * std::pow(2, bufferOctave);
            const int nextReadIndex = resampleSize - (_noteManager.readIndex(key) + audioSpecs().processBlockSize);
            const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;
            // const auto resampleReadSize = (bufferOctave > 0) ? readSize * 2 : readSize / 2;
            const auto resampleReadSize = readSize * std::pow(2, bufferOctave);
            // std::cout << "in: " << _noteManager.readIndex(key) << std::endl;
            // std::cout << "sampleSize: " << sampleSize << std::endl;
            // std::cout << "nextReadIndex: " << nextReadIndex << std::endl;
            // std::cout << "readSize: " << readSize << std::endl;
            // std::cout << "resampleReadSize: " << resampleReadSize << std::endl;
            // std::cout << "resampleSize: " << resampleSize << std::endl;
            // std::cout << "resampleOffset: " << resampleOffset << std::endl;
            // std::cout << std::endl;

            DSP::Resampler<float>().resampleOctave<false, 8u>(sampleBuffer, out, resampleReadSize, audioSpecs().sampleRate, bufferOctave, resampleOffset);
            // DSP::Resampler<float>().resampleOctave<false, 8u>(sampleBuffer + resampleOffset, out, resampleReadSize, audioSpecs().sampleRate, bufferOctave);
            // std::cout << "RESAMPLEED done" << std::endl;

            // Apply enveloppe
            // for (auto i = 0u; i < readSize; ++i) {
            //     const auto idx = _noteManager.readIndex(key) + i;
            //     out[i] *= _noteManager.getEnveloppeGain(key, idx, _noteManager.trigger(key)) * 2;
            // }
            _noteManager.incrementReadIndex(key, resampleSize, readSize);
        }
    }
    // std::cout << "<END>" << std::endl;

}

inline void Audio::Sampler::onAudioGenerationStarted(const BeatRange &range)
{
    _noteManager.reset();
}
