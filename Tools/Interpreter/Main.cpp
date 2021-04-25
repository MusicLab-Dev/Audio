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
    Audio::BufferViews samples;
};

static void Callback(void *userdata, std::uint8_t *data, int size) noexcept
{
    static std::size_t SampleIdx = 0u;
    static std::size_t Idx = 0u;
    static const auto FloatSize = size / 4;
    auto *out = reinterpret_cast<float *>(data);

    auto *sampleData = reinterpret_cast<CallbackData *>(userdata);
    Audio::BufferView sample = sampleData->samples[SampleIdx];
    const auto sampleSize = sample.size<float>();

    for (auto i = 0u; i < FloatSize; ++i) {
        out[i] = sample.data<float>()[Idx];
        ++Idx;
        if (Idx >= sampleSize) {
            // SDL_PauseAudioDevice(2, true);
            // std::cout << "break" << std::endl;
            // return;
            Idx = 0u;
            std::cout << "(" << SampleIdx << ") loop !" << std::endl;
            ++SampleIdx;
            if (SampleIdx >= sampleData->samples.size())
                SampleIdx = 0u;
            sample = sampleData->samples[SampleIdx];
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

#include <Audio/DSP/Filter.hpp>

using namespace Audio;

int main(int ac, char **av)
{
    try {
        if (ac > 1 && (std::string(av[1]) == "-d")) {
            SampleSpecs specs;
            const std::string sampleName = "snare";
            Audio::Buffer sample = SampleManager<float>::LoadSampleFile("Samples/" + sampleName + ".wav", specs);
            const auto sampleSize = sample.size<float>();
            static const DSP::FilterSpecs filter {
                DSP::FilterType::LowPass,
                DSP::WindowType::Custom,
                44100 / 100,
                44100,
                { 44100 / 32 * 4, 0 }
            };

            std::cout << "sampleSize: " << sampleSize << std::endl;
            Audio::Buffer filtered(sampleSize * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);
            filtered.clear();
            // filtered.copy(sample);
            // Filtering::FilterX(filter, sample.data<float>(), filtered.data<float>(), sampleSize, false);
            // Audio::SampleManager<float>::WriteSampleFile("filtewred.wav", filtered);
            // Audio::SampleManager<float>::WriteSampleFile("sample.wav", sample);

            // return 0;

            const auto chunkSize = 1024;
            const std::size_t nChunks = std::ceil(sampleSize / static_cast<float>(chunkSize));
            std::cout << "nChunks: " << nChunks << std::endl;
            std::cout << "sampleSize: " << sampleSize << std::endl;

            Buffer ref0 = SampleManager<float>::LoadSampleFile("ref_0.wav", specs);
            std::vector<float> inStd(chunkSize, 0.f);
            std::vector<float> outStd(chunkSize, 0.f);
            std::memcpy(inStd.data(), ref0.data<float>(), chunkSize * sizeof(float));

            Buffer outBuf(sampleSize * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);
            outBuf.clear();
            Filtering::FilterX(filter, inStd.data(), outStd.data(), chunkSize);
            std::memcpy(outBuf.data<float>(), outStd.data(), chunkSize * sizeof(float));
            SampleManager<float>::WriteSampleFile("tmp.wav", outBuf);

            return 0;

            Buffer ref1 = SampleManager<float>::LoadSampleFile("ref_1.wav", specs);
            Buffer ref2 = SampleManager<float>::LoadSampleFile("ref_2.wav", specs);
            Buffer ref3 = SampleManager<float>::LoadSampleFile("ref_3.wav", specs);
            Buffer ref4 = SampleManager<float>::LoadSampleFile("ref_4.wav", specs);
            BufferViews refs;
            refs.push(ref0);
            refs.push(ref1);
            refs.push(ref2);
            refs.push(ref3);
            refs.push(ref4);

            for (auto i = 0u; i < nChunks; ++i) {
                Buffer out(sampleSize * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);
                out.clear();
                if (!i)
                    Filtering::FilterX(filter, refs[i].data<float>(), out.data<float>() + i * chunkSize, chunkSize);
                else
                    Filtering::FilterLastInputX(filter, refs[i].data<float>(), out.data<float>() + i * chunkSize, chunkSize, refs[i - 1].data<float>(), chunkSize);
                SampleManager<float>::WriteSampleFile("out_" + std::to_string(i) + ".wav", out);

                auto a = 0u;
                // for (auto i = 0u; i < chunkSize; ++i) {
                //     if (out.data<float>()[i] != filtered.data<float>()[i]) {
                //         std::cout << "TAMER: " << i << ": " << out.data<float>()[i] << " != " << filtered.data<float>()[i] << std::endl;
                //         a++;
                //     }
                // }
                std::cout << "a: " << a << std::endl;

                // for (auto i = 0u; i < chunkSize; ++i) {
                //     if (ref0.data<float>()[i] != sample.data<float>()[i]) {
                //         std::cout << "TAMERzzzzz: " << i << ": " << ref0.data<float>()[i] << " != " << sample.data<float>()[i] << std::endl;
                //         a++;
                //     }
                // }
                break;
            }


            return 0;


            for (auto i = 0u; i < nChunks; ++i) {
                const std::int32_t offset = (i + 1) * chunkSize - sampleSize;
                std::vector<float> last(chunkSize + filter.windowSize, 0.f);
                std::vector<float> input(chunkSize + filter.windowSize, 0.f);
                std::memcpy(input.data(), sample.data<float>() + i * chunkSize, chunkSize * sizeof(float));
                Audio::Buffer buf(sampleSize * sizeof(float), 44100, ChannelArrangement::Mono, Format::Floating32);
                buf.clear();
                if (i) {
                    // Save last input
                    std::memcpy(last.data(), sample.data<float>() + (i - 1) * chunkSize, chunkSize * sizeof(float));
                    Filtering::FilterLastInputX(filter, input.data(), buf.data<float>() + i * chunkSize, chunkSize, last.data(), chunkSize);
                    // Filtering::FilterLastInputX(filter, sample.data<float>() + i * chunkSize, buf.data<float>() + i * chunkSize, chunkSize, last.data(), chunkSize);
                } else {
                    Filtering::FilterX(filter, input.data(), buf.data<float>() + i * chunkSize, chunkSize);
                    // Filtering::FilterX(filter, sample.data<float>() + i * chunkSize, buf.data<float>() + i * chunkSize, chunkSize);
                }
                Audio::SampleManager<float>::WriteSampleFile("chunk_" + std::to_string(i) + ".wav", buf);

                std::cout << i * chunkSize << ", " << (i + 1) * chunkSize << " -> " << offset << std::endl;

            }

            return 0;

            if (SDL_InitSubSystem(SDL_INIT_AUDIO))
                throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());
            CallbackData data {{ sample, filtered }};
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