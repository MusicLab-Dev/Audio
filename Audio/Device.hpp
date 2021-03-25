/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include <Core/Functor.hpp>

#include "Base.hpp"

namespace Audio
{
    class Device;

    /** @brief Functor used as an audio callback */
    using AudioCallback = Core::Functor<void(std::uint8_t *, std::size_t)>;
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
    struct alignas_cacheline SDLDescriptor
    {
        std::string         name;
        BlockSize           blockSize { 2048u };
        SampleRate          sampleRate { 48000u };
        bool                isInput { true };
        Format              format { Format::Floating32 };
        MidiChannels        midiChannels { 2u };
        ChannelArrangement  channelArrangement { ChannelArrangement::Mono };
    };

    /** @brief Descibes SDL device beahaviours */
    struct alignas_cacheline DeviceDescriptor
    {
        std::string         name;
        bool                hasInput { false };
        bool                hasOutput { false };
    };


    /** @brief A list of logical device descriptors used to introspect the hardware device */
    using DeviceDescriptors = std::vector<DeviceDescriptor>;
    using DriverDescriptors = std::vector<std::string>;

    /** @brief Construct a device using a descriptor */
    Device(const SDLDescriptor &descriptor, AudioCallback &&callback);

    /** @brief Destroy and release the audio device */
    ~Device(void);


    /** @brief Register the audio callback */
    void start(void) { SDL_PauseAudioDevice(_deviceID, false); }

    /** @brief Unregister the audio callback */
    void stop(void) { SDL_PauseAudioDevice(_deviceID, true); }

    /** @brief Check if the device is running (and audio callback is registered) */
    [[nodiscard]] bool running(void) const noexcept { return (SDL_GetAudioDeviceStatus(_deviceID) == SDL_AUDIO_PLAYING); }


    /** @brief Get/Set the actual device name */
    [[nodiscard]] std::string_view name(void) const noexcept { return _descriptor.name; }
    bool setName(const std::string &name) noexcept;


    /** @brief Get/Set the actual sample rate */
    [[nodiscard]] SampleRate sampleRate(void) const noexcept { return _descriptor.sampleRate; }
    bool setSampleRate(const SampleRate sampleRate) noexcept;


    /** @brief Get/Set the actual format */
    [[nodiscard]] Format format(void) const noexcept { return _descriptor.format; }
    bool setFormat(const Format format) noexcept;


    /** @brief Get/Set the actual midiChannels */
    [[nodiscard]] ChannelArrangement channelArrangement(void) const noexcept { return _descriptor.channelArrangement; }
    bool setChannelArrangement(const ChannelArrangement channelArrangement) noexcept;


    /** @brief Get/Set the actual audio block size */
    [[nodiscard]] std::uint16_t blockSize(void) const noexcept { return _descriptor.blockSize; }
    bool setBlockSize(const std::uint16_t blockSize) noexcept;

    /** @brief Get/Set the actual midi channels */
    [[nodiscard]] std::uint16_t midiChannels(void) const noexcept { return _descriptor.midiChannels; }
    bool setMidiChannels(const MidiChannels midiChannels) noexcept;


    /** @brief Initialize the backend audio driver */
    static void InitDriver(void);

    /** @brief Release the backend audio driver */
    static void ReleaseDriver(void);

    /** @brief Get all driver descriptors */
    static DriverDescriptors GetDriverDescriptors(void);
    static void DebugDriverDescriptors(void);

    /** @brief Get all device descriptors */
    static DeviceDescriptors GetDeviceDescriptors(void);
    static void DebugDeviceDescriptors(void);

    /** @brief Reload the device interface according to the internal descriptor */
    void reloadDevice(void);

    /** @brief Reload the audio driver back-end with a specific driver name. Return true on success */
    [[nodiscard]] bool reloadDriver(const std::string &driverName) noexcept;


private:
    SDLDescriptor _descriptor {};
    AudioCallback _callback {};
    SDL_AudioDeviceID _deviceID {};

    /** @brief Internal audio callback, called by backend */
    static void InternalAudioCallback(void *userdata, std::uint8_t *data, int size) noexcept;
};

#include "Device.ipp"

// static_assert_fit_half_cacheline(Audio::Device);
