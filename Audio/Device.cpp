/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device.cpp
 */

#include <stdexcept>

#include "Device.hpp"

using namespace Audio;

Device::Device(const Descriptor &descriptor, AudioCallback &&callback)
{
    constexpr auto GetFormat = [](const Format format) -> SDL_AudioFormat {
        switch (format) {
        case Format::Floating32:
            return AUDIO_F32;
        case Format::Fixed32:
            return AUDIO_S32;
        case Format::Fixed16:
            return AUDIO_S16;
        case Format::Fixed8:
            return AUDIO_S8;
        default:
            return AUDIO_F32;
        }
    };
    SDL_AudioSpec desiredSpec {};
    desiredSpec.freq = static_cast<int>(descriptor.sampleRate);
    desiredSpec.format = GetFormat(descriptor.format);
    desiredSpec.samples = descriptor.blockSize;
    desiredSpec.callback = callback;
    desiredSpec.userdata = nullptr;
    SDL_AudioSpec acquiredSpec;
    if (!(_deviceID = SDL_OpenAudioDevice(NULL, descriptor.isInput, &desiredSpec, &acquiredSpec, 1)))
        throw std::runtime_error(std::string("Couldn't open audio: ") + SDL_GetError());
}

Device::~Device(void)
{
    SDL_CloseAudioDevice(_deviceID);
}
