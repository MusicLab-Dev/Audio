/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#include <iostream>
#include <SDL2/SDL.h>

#include "Interpreter.hpp"

static const Audio::Device::LogicalDescriptor Descriptor
{
    /*.name = */ "",
    /*.blockSize = */ 1024u,
    /*.sampleRate = */ 44100,
    /*.isInput = */ false,
    /*.format = */ Audio::Format::Floating32,
    /*.midiChannels = */ 2u,
    /*.channelArrangement = */ Audio::ChannelArrangement::Mono
};


struct CallbackData
{
    std::array<Audio::Buffer, 12> *samples;
};


static void Callback(void *userdata, std::uint8_t *data, int size) noexcept
{
    static std::size_t SampleIdx { 3u };
    static std::size_t Idx = 0u;
    static const auto FloatSize = size / 4;
    auto *out = reinterpret_cast<float *>(data);

    auto *sampleData = reinterpret_cast<CallbackData *>(userdata);
    Audio::BufferView sample = (*sampleData->samples)[SampleIdx];
    const auto sampleSize = sample.size<float>();

    for (auto i = 0u; i < FloatSize; ++i) {
        // if (float k = Idx / 500.0; k < 1)
        //     out[i] = sample.data<float>()[Idx] * k;
        // else
            out[i] = sample.data<float>()[Idx];
        // if (float k = (Idx - (*sampleData->samples).size() + 1000) / 1000.0; k < 1)
        //     out[i] = sample.data<float>()[Idx] * (k);
        // else
        //     out[i] = sample.data<float>()[Idx];

        ++Idx;
        if (Idx >= sampleSize) {
            Idx = 0u;
            ++SampleIdx;
            if (SampleIdx >= (*sampleData->samples).size())
                SampleIdx = 0u;
            std::cout << "loop !" << std::endl;
        }
    }
}

static std::size_t Init(CallbackData *data)
{
    constexpr auto GetFormat = [](const Audio::Format format) -> SDL_AudioFormat {
        switch (format) {
        case Audio::Format::Unknown:
            return 0;
        case Audio::Format::Floating32:
            return AUDIO_F32;
        case Audio::Format::Fixed32:
            return AUDIO_S32;
        case Audio::Format::Fixed16:
            return AUDIO_S16;
        case Audio::Format::Fixed8:
            return AUDIO_S8;
        default:
            return AUDIO_F32;
        }
    };

    SDL_AudioSpec desiredSpec {};
    desiredSpec.freq = static_cast<std::size_t>(Descriptor.sampleRate);
    desiredSpec.format = GetFormat(Descriptor.format);
    desiredSpec.samples = Descriptor.blockSize;
    desiredSpec.callback = &Callback;
    desiredSpec.userdata = data;
    desiredSpec.channels = static_cast<std::size_t>(Descriptor.channelArrangement);
    SDL_AudioSpec acquiredSpec;

    std::size_t device = 0;
    if (!(device = SDL_OpenAudioDevice(NULL, Descriptor.isInput, &desiredSpec, &acquiredSpec, 1)))
        throw std::runtime_error(std::string("Couldn't open audio: ") + SDL_GetError());
    return device;
}

#include <Audio/SampleFile/SampleManager.hpp>
#include <Audio/BufferOctave.hpp>
// #include <Audio/KissFFT.hpp>
#include <Audio/DSP/FIR.hpp>


using namespace Audio;

int main(int ac, char **av)
{
    try {
        if (ac > 1 && (std::string(av[1]) == "-d")) {
            SampleSpecs specs;
            std::array<Audio::Buffer, 12> resampled;
            const std::string sampleName = "Snare";
            resampled[OctaveRootKey] = SampleManager<float>::LoadSampleFile("Samples/" + sampleName + ".wav", specs);
            const auto sampleSize = resampled[OctaveRootKey].size<float>();
            GenerateOctave<float>(resampled[OctaveRootKey], resampled);
            for (auto i = 0; i < OctaveRootKey; ++i) {
                const auto idx = OctaveRootKey - i - 1;
                std::cout << "id: " << idx << std::endl;
                if (auto done = SampleManager<float>::WriteSampleFile("Samples/FIR/" + sampleName + "_down_" + std::to_string(i + 1) + ".wav", resampled[idx]); !done)
                    std::cout << "failed write: " << idx << std::endl;
            }
            if (SDL_InitSubSystem(SDL_INIT_AUDIO))
                throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());
            CallbackData data { &resampled };
            auto device = Init(&data);
            SDL_PauseAudioDevice(device, false);
            while (true);
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            SDL_Quit();

        } else {
            Device::DriverInstance driverInstance;
            PluginTable::Instance pluginTableInstance;
            Interpreter interpreter;

            interpreter.run();
        }

        return 0;
    } catch (const std::exception &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}