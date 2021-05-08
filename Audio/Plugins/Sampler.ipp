/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/BufferOctave.hpp>
#include <Audio/SampleFile/SampleManager.hpp>

#include <Audio/DSP/FIR.hpp>

template<typename Type>
inline void Audio::Sampler::loadSample(const std::string_view &path)
{
    SampleSpecs fileSpecs {};
    SampleSpecs desiredSpecs {
        audioSpecs().sampleRate,
        audioSpecs().channelArrangement,
        audioSpecs().format,
        0u
    };
    _buffers[OctaveRootKey] = SampleManager<Type>::LoadSampleFile(std::string(path), desiredSpecs, fileSpecs);

    GenerateOctave<Type>(_buffers[OctaveRootKey], _buffers);

    // static DSP::FIRFilter<float> filter;
    // filter.setSpecs(DSP::Filter::FIRSpecs {
    //     DSP::Filter::BasicType::LowPass,
    //     DSP::Filter::WindowType::Hanning,
    //     255ul,
    //     static_cast<double>(audioSpecs().sampleRate),
    //     { 1000.0, 0.0 },
    //     1.0
    // });

    // BufferView ref(_buffers[OctaveRootKey]);
    // Buffer filtered(ref.channelByteSize(), ref.sampleRate(), ref.channelArrangement(), ref.format());
    // filter.filter<false>(ref.data<float>(), ref.size<float>(), filtered.data<float>());
    // SampleManager<float>::WriteSampleFile("filtered.wav", filtered);
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
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()));
    const std::uint32_t outSize = static_cast<std::uint32_t>(output.size<float>());
    float *out = reinterpret_cast<float *>(output.byteData());

    _noteManager.processNotes(
        [this, outGain, outSize, out](const Key key, const bool trigger, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t> {
            const std::int32_t realKeyIdx = static_cast<std::int32_t>(key) % KeysPerOctave;
            const std::int32_t realOctave = static_cast<std::int32_t>(key) / KeysPerOctave;
            std::int32_t bufferKeyIdx = realKeyIdx - OctaveRootKey + 1;
            std::int32_t bufferOctave;

            // Get key idx in buffer
            if (bufferKeyIdx < 0) {
                bufferOctave = realOctave - RootOctave - 1;
                bufferKeyIdx += KeysPerOctave;
            } else
                bufferOctave = realOctave - RootOctave;

            auto * const sampleBuffer = _buffers[bufferKeyIdx].data<float>();
            const auto sampleSize = static_cast<std::uint32_t>(_buffers[bufferKeyIdx].size<float>());

            // The key is already cached
            if (!bufferOctave) {
                const int nextReadIndex = static_cast<int>(sampleSize - (readIndex + audioSpecs().processBlockSize));
                auto readSize = nextReadIndex < 0 ? outSize - static_cast<std::uint32_t>(-nextReadIndex) : outSize;
                auto realOut = out;

                if (trigger) {
                    realOut += modifiers.sampleOffset;
                    readSize -= modifiers.sampleOffset;
                } else
                    readSize = modifiers.sampleOffset;
                // Apply enveloppe
                for (auto i = 0u; i < readSize; ++i) {
                    const auto idx = readIndex + i;
                    realOut[i] += sampleBuffer[idx] * getEnveloppeGain(key, idx, trigger) * outGain;
                }
                return std::make_pair(readSize, sampleSize);
            // The key need an octave shift
            } else {
                const std::uint32_t resampleSize = static_cast<std::uint32_t>(static_cast<float>(sampleSize) * std::pow(2, -bufferOctave));
                const std::uint32_t resampleOffset = static_cast<std::uint32_t>(static_cast<float>(_noteManager.readIndex(key)) * std::pow(2, bufferOctave));
                const int nextReadIndex = static_cast<int>(resampleSize - (_noteManager.readIndex(key) + audioSpecs().processBlockSize));
                const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;
                std::uint32_t resampleReadSize = static_cast<std::uint32_t>(static_cast<float>(readSize) * std::pow(2, bufferOctave));
                auto realOut = out;

                if (trigger) {
                    realOut += modifiers.sampleOffset;
                    resampleReadSize -= modifiers.sampleOffset;
                } else
                    resampleReadSize = modifiers.sampleOffset;

                DSP::Resampler<float>().resampleOctave<true, 8u>(sampleBuffer, realOut, resampleReadSize, audioSpecs().sampleRate, bufferOctave, resampleOffset);

                // Apply enveloppe
                for (auto i = 0u; i < readSize; ++i) {
                    realOut[i] *= getEnveloppeGain(key, readIndex + i, trigger) * outGain;
                }
                return std::make_pair(resampleReadSize, resampleSize);
            }
        }
    );
}

inline void Audio::Sampler::onAudioGenerationStarted(const BeatRange &)
{
    _noteManager.reset();
}
