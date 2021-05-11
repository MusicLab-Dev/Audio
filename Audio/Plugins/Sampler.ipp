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
    float * const out = reinterpret_cast<float *>(output.byteData());
    const bool noRelease = !enveloppeRelease();

    _noteManager.processNotes(
        [this, outGain, outSize, out, noRelease](const Key key, const bool trigger, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t> {
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
            auto realOutSize = outSize;
            auto realOut = out;

            // Handle note end
            if (modifiers.sampleOffset) {
                if (trigger) {
                    realOut += modifiers.sampleOffset;
                    realOutSize -= modifiers.sampleOffset;
                } else if (noRelease) {
                    realOutSize = modifiers.sampleOffset;
                }
            }
            // The key is already cached
            if (!bufferOctave) {
                // Handle sample end
                const auto samplesLeft = sampleSize - readIndex;
                realOutSize = std::min(samplesLeft, realOutSize);
                // Apply enveloppe
                for (auto i = 0u, j = readIndex; i < realOutSize; ++i, ++j) {
                    // realOut[i] += sampleBuffer[j] * outGain;
                    realOut[i] += sampleBuffer[j] * getEnveloppeGain(key, j, trigger) * outGain;
                }
                return std::make_pair(realOutSize, sampleSize);
            // The key need an octave shift
            } else {
                const auto bufferOctaveShift = std::pow(2.0, -bufferOctave);
                const auto bufferOctaveReverseShift = std::pow(2.0, bufferOctave);
                const std::uint32_t resampleSize = static_cast<std::uint32_t>(static_cast<float>(sampleSize) * bufferOctaveShift);
                const std::uint32_t sampleOffset = static_cast<std::uint32_t>(static_cast<float>(readIndex) * bufferOctaveReverseShift);
                // Handle sample end
                const auto resamplesLeft = resampleSize - readIndex;
                realOutSize = std::min(resamplesLeft, realOutSize);
                const std::uint32_t sampleReadSize = static_cast<std::uint32_t>(static_cast<float>(realOutSize) * bufferOctaveReverseShift);
                // Create resampled version of the base octave
                DSP::Resampler<float>().resampleOctave<true, 8u>(sampleBuffer, realOut, sampleReadSize, audioSpecs().sampleRate, bufferOctave, sampleOffset);

                /** @warning FIX PROBLEMS */
                // Apply enveloppe
                for (auto i = 0u, j = readIndex; i < realOutSize; ++i, ++j) {
                    realOut[i] *= getEnveloppeGain(key, j, trigger) * outGain;
                }
                return std::make_pair(realOutSize, resampleSize);
            }
        }
    );
}

inline void Audio::Sampler::onAudioGenerationStarted(const BeatRange &)
{
    _noteManager.reset();
}
