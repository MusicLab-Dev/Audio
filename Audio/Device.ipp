/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device
 */

inline void Audio::Device::InitDriver(void)
{
    if (SDL_Init(0))
        throw std::runtime_error(std::string("Couldn't initialize SDL: ") + SDL_GetError());
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());
}

inline void Audio::Device::ReleaseDriver(void)
{
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
}

inline bool Audio::Device::setSampleRate(const int sampleRate) noexcept
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
