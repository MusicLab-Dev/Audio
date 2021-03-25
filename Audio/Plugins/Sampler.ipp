/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/BufferOctave.hpp>
#include <Audio/SampleFile/SampleManager.hpp>

template<typename Type>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    SampleSpecs specs;
    _buffers[OctaveRootKey] = SampleManager<Type>::LoadSampleFile(path, specs);
    GenerateOctave<Type>(_buffers[OctaveRootKey], _buffers);
}

inline void Audio::Sampler::onAudioParametersChanged(void)
{
    // Buffer sourceBuffer = _buffers[0];

    for (auto &buffer : _buffers) {
        const auto newSize = GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize;
        buffer.resize(newSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
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
    // output.clear();
    // static bool Bool = false;
    // Bool = !Bool;
    // if (!Bool)
    //     return;
    // std::cout << "))=)======\n======\n======\n======\n=@@@@@@\n@@@@@@\n@@@@@@\n@@@@@@\n@@@@@@\n@@@\n";
    // std::cout << "AUDIO\n";
    // std::cout << "receiveAudio:::: " << _noteManager.getActiveNoteNumber() << std::endl;

    const auto outSize = output.size<float>();
    // std::cout << "Sampler::output.byteData(): " << outSize << " " << reinterpret_cast<const int *>(output.byteData()) << std::endl;
    float *out = reinterpret_cast<float *>(output.byteData());
    // std::uint8_t *out = output.byteData();


    float *sampleBuffer = nullptr;
    std::size_t sampleSize = 0u;
    // std::cout << "Sampler::receiveAudio::sampleSize: " << sampleSize << std::endl;

    // bool hasProcess = false;

    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        // const std::size_t bufferIdx = OctaveRootKey - RootKey + key;
        const std::size_t bufferIdx = OctaveRootKey;
        // std::cout << "idx: " << _noteManager.readIndex(key) << " triger: " << _noteManager.trigger(key) << std::endl;
        // if (!_noteManager.trigger(key))
        //     continue;
        sampleBuffer = _buffers[bufferIdx].data<float>();
        sampleSize = _buffers[bufferIdx].size<float>();
        // hasProcess = true;
        for (auto i = 0u; i < outSize; ++i) {
            out[i] += _outputGain * sampleBuffer[_noteManager.readIndex(key)] / static_cast<float>(activeNote);
            _noteManager.incrementReadIndex(key, sampleSize);
        }
    }
    // for (auto &key : _noteManager.getActiveNoteBlock()) {

        // std::cout << "note block:" << std::endl;
        // for (auto i = 0u; i < outSize; ++i) {
        //     output.data<float>(static_cast<Channel>(iChannel))[i] += _outputGain * _buffers[0].data<float>(static_cast<Channel>(iChannel))[_readIndex[key]];
        //     incrementReadIndex(key);
        // }
    // }
    //     output.clear();
    // if (hasProcess)
    //     std::cout << " - HAS PROCESS: " << hasProcess << std::endl;
}
