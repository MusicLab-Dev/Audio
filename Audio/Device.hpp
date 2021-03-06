/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "Base.hpp"

namespace Audio
{
    class Device;

    // Forward declaration to the SDL audio callback function signature
    using AudioCallback = SDL_AudioCallback;
};


/** @brief Device represent an SDL audio device, it can be used either for input OR output */
class alignas_half_cacheline Audio::Device
{
public:
    /** @brief Helper used to init and release the driver instance */
    class DriverInstance
    {
    public:
        DriverInstance(void) { Device::InitDriver(); }
        ~DriverInstance(void) { Device::ReleaseDriver(); }
    };

    /** @brief Descibes how to use a logical device and is used to retreive one */
    struct alignas_cacheline Descriptor
    {
        std::string         name { "untitled" };
        BlockSize           blockSize { 2048u };
        SampleRate          sampleRate { 48000u };
        bool                isInput { true };
        Format              format { Format::Floating32 };
        MidiChannels        midiChannels { 2u };
        ChannelArrangement  channelArrangement { ChannelArrangement::Mono };
    };


    /** @brief A list of logical device descriptors used to introspect the hardware device */
    using Descriptors = std::vector<Descriptor>;

    /** @brief Construct a device using a descriptor */
    Device(const Descriptor &descriptor, AudioCallback &&callback);

    /** @brief Destroy and release the audio device */
    ~Device(void);


    /** @brief Register the audio callback */
    void start(void) { SDL_PauseAudioDevice(_deviceID, false); }

    /** @brief Unregister the audio callback */
    void stop(void) { SDL_PauseAudioDevice(_deviceID, true); }

    /** @brief Check if the device is running (and audio callback is registered) */
    [[nodiscard]] bool running(void) const noexcept { return (SDL_GetAudioDeviceStatus(_deviceID) == SDL_AUDIO_PLAYING); }


    /** @brief Get the actual format */
    [[nodiscard]] std::string_view name(void) const noexcept { return _descriptor.name; }


    /** @brief Get the actual sample rate */
    [[nodiscard]] int sampleRate(void) const noexcept { return _descriptor.sampleRate; }

    /** @brief Set the sample rate, return true if the value changed */
    bool setSampleRate(const int sampleRate) noexcept;


    /** @brief Get the actual format */
    [[nodiscard]] Format format(void) const noexcept { return _descriptor.format; }

    /** @brief Set the format, return true if the value changed */
    bool setFormat(const Format format) noexcept;


    /** @brief Get the actual midiChannels */
    [[nodiscard]] ChannelArrangement channelArrangement(void) const noexcept { return _descriptor.channelArrangement; }

    /** @brief Set the channelArrangement, return true if the value changed */
    bool setChannelArrangement(const ChannelArrangement channelArrangement) noexcept;


    /** @brief Get the actual audio block size */
    [[nodiscard]] std::uint16_t blockSize(void) const noexcept { return _descriptor.blockSize; }

    /** @brief Set the audio block size, return true if the value changed */
    bool setBlockSize(const std::uint16_t blockSize) noexcept;

    /** @brief Get the actual audio block size */
    [[nodiscard]] std::uint16_t midiChannels(void) const noexcept { return _descriptor.midiChannels; }

    /** @brief Set the audio block size, return true if the value changed */
    bool setMidiChannels(const MidiChannels midiChannels) noexcept;


    /** @brief Initialize the backend audio driver */
    static void InitDriver(void);

    /** @brief Release the backend audio driver */
    static void ReleaseDriver(void);

    /** @brief Get all device descriptors */
    static Descriptors GetDeviceDescriptors(void);

    void reloadDriver(AudioCallback &&callback);

private:
    SDL_AudioDeviceID   _deviceID {};
    AudioCallback       _callback { nullptr };
    Descriptor          _descriptor {};

};

#include "Device.ipp"

// static_assert_fit_half_cacheline(Audio::Device);
