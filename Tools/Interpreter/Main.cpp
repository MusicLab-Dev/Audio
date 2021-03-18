/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#include <iostream>

#include "Interpreter.hpp"

static const Audio::Device::Descriptor Descriptor
{
    /*.name = */ "device-test",
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
    static std::size_t SampleIdx { 0u };
    static std::size_t Idx = 0u;
    static const auto FloatSize = size / 4;
    auto *out = reinterpret_cast<float *>(data);

    auto *sampleData = reinterpret_cast<CallbackData *>(userdata);
    Audio::BufferView sample = (*sampleData->samples)[SampleIdx];
    const auto sampleSize = sample.size<float>();

    for (auto i = 0u; i < FloatSize; ++i) {
        out[i] = sample.data<float>()[++Idx] /* / (5 - SampleIdx) / 2 */;

        if (Idx >= sampleSize) {
            Idx = 0u;
            ++SampleIdx;
            if (SampleIdx >= (*sampleData->samples).size() - 7)
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
// #include <Audio/KissFFT.hpp>
#include <Audio/DSP/FIR.hpp>


using namespace Audio;

int main(void)
{
    try {
        if (true) {
            SampleSpecs specs;
            const auto RootKey = 5u;
            std::array<Audio::Buffer, 12> resampled;
            resampled[RootKey] = SampleManager<float>::LoadSampleFile("Samples/chord.wav", specs);
            const auto sampleSize = resampled[RootKey].size<float>();

            // Lower notes
            auto lastSize = sampleSize;
            auto lastIdx = RootKey;
            // auto min = std::min_element(resampled[lastIdx].data<float>(), (resampled[lastIdx].data<float>() + sampleSize));
            // auto max = std::max_element(resampled[lastIdx].data<float>(), (resampled[lastIdx].data<float>() + sampleSize));
            // std::cout << "  :::: " << *min << ", " << *max << std::endl;
            for (auto i = 0u; i < RootKey; ++i) {
                resampled[lastIdx - 1].resize(lastSize * sizeof(float), resampled[lastIdx].sampleRate(), resampled[lastIdx].channelArrangement(), resampled[lastIdx].format());
                DSP::FIR::Resample<8u>(resampled[lastIdx].data<float>(), resampled[lastIdx - 1].data<float>(), lastSize, 44100, 196, 185);
                // auto minF = std::min_element(resampled[lastIdx - 1].data<float>(), (resampled[lastIdx - 1].data<float>() + sampleSize));
                // auto maxF = std::max_element(resampled[lastIdx - 1].data<float>(), (resampled[lastIdx - 1].data<float>() + sampleSize));
                // std::cout << "  :::: " << *minF << ", " << *maxF << std::endl;

                lastIdx -= 1;
                lastSize = DSP::Resampler<float>::GetResamplingSizeOneSemitone(lastSize, false);
                // break;
            }


            // Buffer resampled(bufferSize, 44100, ChannelArrangement::Mono, Format::Floating32);
            // DSP::Resampler<float>::ResampleSemitone(sample.data<float>(), sampleFilter.data<float>(), sampleSize, false);
            // DSP::FIR::Resample(sample.data<float>(), sampleFilter.data<float>(), sampleSize, 44100, 4, 3);
            // DSP::FIR::Resample(originalSample.data<float>(), sampleFilter.data<float>(), sampleSize, 44100, 185, 195);

            // return 0;

            if (SDL_InitSubSystem(SDL_INIT_AUDIO))
                throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());

            // auto min = std::min_element(originalSample.data<float>(), (originalSample.data<float>() + sampleSize));
            // auto max = std::max_element(originalSample.data<float>(), (originalSample.data<float>() + sampleSize));
            // std::cout << *min << ", " << *max << std::endl;
            // auto minF = std::min_element(sampleFilter.data<float>(), (sampleFilter.data<float>() + sampleSize));
            // auto maxF = std::max_element(sampleFilter.data<float>(), (sampleFilter.data<float>() + sampleSize));
            // std::cout << *minF << ", " << *maxF << std::endl;

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