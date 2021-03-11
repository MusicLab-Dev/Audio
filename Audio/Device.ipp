/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device
 */

#include <iostream>

inline void Audio::Device::InitDriver(void)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());
}

inline void Audio::Device::ReleaseDriver(void)
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
}

inline void Audio::Device::reloadDriver(AudioCallback &&callback)
{
    std::cout << "Reload driver" << std::endl;
    constexpr auto GetFormat = [](const Format format) -> SDL_AudioFormat {
        switch (format) {
        case Format::Unknown:
            return 0;
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
    desiredSpec.freq = static_cast<std::size_t>(_descriptor.sampleRate);
    desiredSpec.format = GetFormat(_descriptor.format);
    desiredSpec.samples = _descriptor.blockSize;
    desiredSpec.callback = callback;
    desiredSpec.userdata = nullptr;
    desiredSpec.channels = static_cast<std::size_t>(_descriptor.channelArrangement);
    SDL_AudioSpec acquiredSpec;

    if (!(_deviceID = SDL_OpenAudioDevice(NULL, _descriptor.isInput, &desiredSpec, &acquiredSpec, 1)))
        throw std::runtime_error(std::string("Couldn't open audio: ") + SDL_GetError());
}

inline bool Audio::Device::setSampleRate(const SampleRate sampleRate) noexcept
{
    if (sampleRate == _descriptor.sampleRate)
        return false;
    _descriptor.sampleRate = sampleRate;
    return true;
}

inline bool Audio::Device::setFormat(const Format format) noexcept
{
    if (format == _descriptor.format)
        return false;
    _descriptor.format = format;
    return true;
}

inline bool Audio::Device::setChannelArrangement(const ChannelArrangement channelArrangement) noexcept
{
    if (channelArrangement == _descriptor.channelArrangement)
        return false;
    _descriptor.channelArrangement = channelArrangement;
    return true;
}

inline bool Audio::Device::setBlockSize(const std::uint16_t blockSize) noexcept
{
    if (blockSize == _descriptor.blockSize)
        return false;
    _descriptor.blockSize = blockSize;
    return true;
}

inline bool Audio::Device::setMidiChannels(const MidiChannels midiChannels) noexcept
{
    if (midiChannels == _descriptor.midiChannels)
        return false;
    _descriptor.midiChannels = midiChannels;
    return true;
}
