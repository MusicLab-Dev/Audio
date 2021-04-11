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
    const auto inSize = _buffers[0].size<float>();
    _tmp.resize(inSize / 2 * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);

    DSP::Resampler::ResampleOctave<false>(
        _buffers[0].data<float>(),
        _tmp.data<float>(),
        inSize,
        44100,
        1
    );

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

    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        // if (!_noteManager.trigger(key))
        //     continue;
        const std::int32_t bufferIdxDt = static_cast<std::int32_t>(key) - static_cast<std::int32_t>(RootKey) + OctaveRootKey;
        const std::size_t bufferIdx = (bufferIdxDt >= 0) ? bufferIdxDt % KeysPerOctave : (KeysPerOctave + bufferIdxDt) % KeysPerOctave;
        const std::int32_t nOctave = (bufferIdxDt >= 0) ? bufferIdxDt / KeysPerOctave : -((KeysPerOctave - bufferIdxDt - 1) / KeysPerOctave);


        // std::cout << "buffer idx dt: " << bufferIdxDt << std::endl;
        // std::cout << "buffer idx: " << bufferIdx << std::endl;
        // std::cout << "nOctave: " << nOctave << std::endl;

        sampleBuffer = _buffers[bufferIdx].data<float>();
        sampleSize = _buffers[bufferIdx].size<float>();
        // sampleBuffer = _tmp.data<float>();
        // sampleSize = _tmp.size<float>();
        // return;

        // std::cout << "in: " << _noteManager.readIndex(key) << std::endl;
        // std::cout << "sampleSize: " << sampleSize << std::endl;
        // std::cout << "nextReadIndex: " << nextReadIndex << std::endl;
        // std::cout << "readSize: " << readSize << std::endl;
        // std::cout << std::endl;
        if (!nOctave) {
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
            sampleBuffer = _buffers[bufferIdx].data<float>();
            sampleSize = _buffers[bufferIdx].size<float>();
            // const std::size_t resampleSize = (nOctave > 0) ? sampleSize / 2 : sampleSize * 2;
            const std::size_t resampleSize = sampleSize * std::pow(2, -nOctave);
            // const std::size_t resampleOffset = (nOctave < 0) ? _noteManager.readIndex(key) / 2 : _noteManager.readIndex(key) * 2;
            const std::size_t resampleOffset = _noteManager.readIndex(key) * std::pow(2, nOctave);
            const int nextReadIndex = resampleSize - (_noteManager.readIndex(key) + audioSpecs().processBlockSize);
            const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;
            // const auto resampleReadSize = (nOctave > 0) ? readSize * 2 : readSize / 2;
            const auto resampleReadSize = readSize * std::pow(2, nOctave);
            // std::cout << "in: " << _noteManager.readIndex(key) << std::endl;
            // std::cout << "sampleSize: " << sampleSize << std::endl;
            // std::cout << "nextReadIndex: " << nextReadIndex << std::endl;
            // std::cout << "readSize: " << readSize << std::endl;
            // std::cout << "resampleReadSize: " << resampleReadSize << std::endl;
            // std::cout << "resampleSize: " << resampleSize << std::endl;
            // std::cout << "resampleOffset: " << resampleOffset << std::endl;
            // std::cout << std::endl;
            DSP::Resampler::ResampleOctave<true>(sampleBuffer, out, resampleOffset + resampleReadSize, audioSpecs().sampleRate, nOctave, resampleOffset);
            // std::cout << "RESAMPLEED done" << std::endl;

            // Apply enveloppe
            // for (auto i = 0u; i < readSize; ++i) {
            //     const auto idx = _noteManager.readIndex(key) + i;
            //     out[i] *= _noteManager.getEnveloppeGain(key, idx, _noteManager.trigger(key)) * 2;
            // }
            _noteManager.incrementReadIndex(key, resampleSize, readSize);
        }
    }

    static const DSP::FilterSpecs filter {
        DSP::FilterType::LowPass,
        DSP::WindowType::Hanning,
        441,
        44100,
        { 5512 * 2, 0 }
    };

    // std::cout << "<END>" << std::endl;
    static bool LastBlockActive = false;
    static auto Cpt = 0u;
    static Buffer lastBlock(outSize * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);
    // lastBlock.clear();

    if (LastBlockActive) {
        DSP::FIR::FilterLastInput(filter, out, out, outSize, lastBlock.data<float>(), outSize);
        std::memcpy(lastBlock.data<float>(), out, outSize * sizeof(float));
    } else {
        DSP::FIR::Filter(filter, out, out, outSize);
    }
    // std::cout << "<END>" << std::endl;
    // std::memcpy(out, tmp.data(), outSize * sizeof(float));

    LastBlockActive = true;
    Cpt++;
}

inline void Audio::Sampler::onAudioGenerationStarted(const BeatRange &range)
{
    _noteManager.reset();
}
