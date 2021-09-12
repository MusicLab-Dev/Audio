/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/BufferOctave.hpp>
#include <Audio/SampleFile/SampleManager.hpp>

#include <Audio/DSP/FIR.hpp>

inline void Audio::Sampler::onAudioGenerationStarted(const BeatRange &)
{
    _noteManager.reset();
}

inline void Audio::Sampler::onAudioParametersChanged(void)
{
    if (!_externalPaths.empty())
        loadSample<float>(_externalPaths[0]);
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Sampler::setExternalPaths(const ExternalPaths &paths)
{
    _externalPaths = paths;
    if (!_externalPaths.empty())
        loadSample<float>(_externalPaths[0]);
}

inline void Audio::Sampler::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    _noteManager.feedNotes(notes);
}

inline void Audio::Sampler::receiveAudio(BufferView output)
{
    if (_externalPaths.empty())
        return;

    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()));
    const std::uint32_t outSize = static_cast<std::uint32_t>(output.size<float>());
    float * const out = reinterpret_cast<float *>(output.byteData());

    _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
        0.0f,
        static_cast<float>(attack()),
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        static_cast<float>(release()),
    });
    _noteManager.envelopeGain().resize(outSize);
    _noteManager.processNotesEnvelope(
        output,
        [this, outGain, outSize, out](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            UNUSED(channelCount);
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

            auto usedOutSize = realOutSize;
            // The key is already cached
            if (!bufferOctave) {
                // Handle sample end
                const auto samplesLeft = sampleSize - readIndex;
                usedOutSize = std::min(samplesLeft, usedOutSize);
                // Apply enveloppe
                for (auto i = 0u, j = readIndex; i < usedOutSize; ++i, ++j) {
                    realOutput[i] += sampleBuffer[j] * _noteManager.envelopeGain()[i] * outGain;
                }
                return std::make_pair(usedOutSize, sampleSize);
            // The key need an octave shift
            } else {
                const auto bufferOctaveShift = std::pow(2.0, -bufferOctave);
                const auto bufferOctaveReverseShift = std::pow(2.0, bufferOctave);
                const std::uint32_t resampleSize = static_cast<std::uint32_t>(static_cast<float>(sampleSize) * bufferOctaveShift);
                const std::uint32_t sampleOffset = static_cast<std::uint32_t>(static_cast<float>(readIndex) * bufferOctaveReverseShift);
                // Handle sample end
                const auto resamplesLeft = resampleSize - readIndex;
                usedOutSize = std::min(resamplesLeft, usedOutSize);
                const std::uint32_t sampleReadSize = static_cast<std::uint32_t>(static_cast<float>(usedOutSize) * bufferOctaveReverseShift);
                // Create resampled version of the base octave
                DSP::Resampler<float>().resampleOctave<true, 8u>(sampleBuffer, realOutput, sampleReadSize, audioSpecs().sampleRate, bufferOctave, sampleOffset);

                /** @warning FIX PROBLEMS */
                // Apply enveloppe
                for (auto i = 0u, j = readIndex; i < usedOutSize; ++i, ++j) {
                    realOutput[i] *= _noteManager.envelopeGain()[i] * outGain;
                }
                return std::make_pair(usedOutSize, resampleSize);
            }
        },
        [this] (const Key key) -> bool
        {
            return !_noteManager.envelope().lastGain(key);
        },
        [this] (const Key key) -> void
        {
            UNUSED(key);
        }
    );
}

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
