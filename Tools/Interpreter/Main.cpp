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

static void Callback(void *userdata, std::uint8_t *data, int size) noexcept
{
    static std::size_t idx = 0u;
    static const auto fSize = size / 4;
    auto *out = reinterpret_cast<float *>(data);
    auto *sample = reinterpret_cast<Audio::Buffer *>(userdata);
    const auto sampleSize = sample->size<float>();

    for (auto i = 0u; i < fSize; ++i) {
        out[i] = sample->data<float>()[++idx];

        if (idx >= sampleSize - 1413) {
            idx = 0u;
            // std::cout << "loop !" << std::endl;
        }
    }

}

static std::size_t Init(Audio::Buffer *sample)
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
    desiredSpec.userdata = sample;
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
        if (false) {
            SampleSpecs specs;
            auto sample = SampleManager<float>::LoadSampleFile("Samples/chord.wav", specs);
            const auto sampleSize = sample.size<float>();
            Buffer sampleFilter;
            sampleFilter.resize(sample.channelByteSize(), sample.sampleRate(), sample.channelArrangement(), sample.format());

            // KissFFT::Engine::Filter(KissFFT::FirFilterSpecs {
            //     KissFFT::FirFilterType::LowPass,
            //     DSP::WindowType::Hanning,
            //     44100,
            //     { 3500, 0 }
            // }, sample.data<float>(), sampleFilter.data<float>(), sampleSize);

            DSP::FIR::Filter(DSP::FilterSpecs {
                DSP::FilterType::LowPass,
                DSP::WindowType::Hanning,
                131,
                44100,
                { 3333, 0 }
            }, sample.data<float>(), sampleSize, sample.data<float>() );

            // return 0;

            std::cout << "sampleSize:" << sampleFilter.size<float>() << std::endl;
            if (SDL_InitSubSystem(SDL_INIT_AUDIO))
                throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());

            auto min = std::min_element(sampleFilter.data<float>(), (sampleFilter.data<float>() + sampleSize));
            auto max = std::max_element(sampleFilter.data<float>(), (sampleFilter.data<float>() + sampleSize));
            std::cout << *min << ", " << *max << std::endl;

            auto device = Init(&sample);
            // auto device = Init(&sampleFilter);
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