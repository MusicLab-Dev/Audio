/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device
 */

#pragma once

#include <memory>
#include <vector>

#include <Core/Functor.hpp>
#include <Core/String.hpp>

#include "Base.hpp"

namespace Audio
{
    class Device;

    /** @brief Device ID */
    using DeviceID = std::uint32_t;

    /** @brief Functor used as an audio callback */
    using AudioCallback = Core::Functor<void(std::uint8_t *, std::size_t)>;
};


/** @brief Device represent an SDL audio device, it can be used either for input OR output */
class alignas_double_cacheline Audio::Device
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
    struct alignas_cacheline LogicalDescriptor
    {
        Core::TinyString    name {};
        BlockSize           blockSize { 2048u };
        SampleRate          sampleRate { 48000u };
        bool                isInput { true };
        Format              format { Format::Floating32 };
        MidiChannels        midiChannels { 2u };
        ChannelArrangement  channelArrangement { ChannelArrangement::Mono };
    };

    /** @brief Descibes SDL device beahaviours */
    struct alignas_cacheline PhysicalDescriptor
    {
        Core::TinyString    name {};
        bool                hasInput { false };
        bool                hasOutput { false };
    };


    /** @brief A list of logical device descriptors used to introspect the hardware device */
    using PhysicalDescriptors = std::vector<PhysicalDescriptor>;
    using DriverDescriptors = std::vector<Core::TinyString>;

    /** @brief Construct a device using a descriptor */
    Device(const LogicalDescriptor &descriptor, AudioCallback &&callback);

    /** @brief Destroy and release the audio device */
    ~Device(void);


    /** @brief Register the audio callback */
    void start(void);

    /** @brief Unregister the audio callback */
    void stop(void);

    /** @brief Check if the device is running (and audio callback is registered) */
    [[nodiscard]] bool running(void) const noexcept;


    /** @brief Get/Set the actual device name */
    [[nodiscard]] const Core::TinyString &name(void) const noexcept { return _descriptor.name; }
    void setName(const Core::TinyString &name) noexcept { _descriptor.name = name; }


    /** @brief Get/Set the actual sample rate */
    [[nodiscard]] SampleRate sampleRate(void) const noexcept { return _descriptor.sampleRate; }
    void setSampleRate(const SampleRate sampleRate) noexcept { _descriptor.sampleRate = sampleRate; }


    /** @brief Get/Set the actual format */
    [[nodiscard]] Format format(void) const noexcept { return _descriptor.format; }
    void setFormat(const Format format) noexcept { _descriptor.format = format; }


    /** @brief Get/Set the actual midiChannels */
    [[nodiscard]] ChannelArrangement channelArrangement(void) const noexcept { return _descriptor.channelArrangement; }
    void setChannelArrangement(const ChannelArrangement channelArrangement) noexcept { _descriptor.channelArrangement = channelArrangement; }


    /** @brief Get/Set the actual audio block size */
    [[nodiscard]] std::uint16_t blockSize(void) const noexcept { return _descriptor.blockSize; }
    void setBlockSize(const std::uint16_t blockSize) noexcept { _descriptor.blockSize = blockSize; }

    /** @brief Get/Set the actual midi channels */
    [[nodiscard]] std::uint16_t midiChannels(void) const noexcept { return _descriptor.midiChannels; }
    void setMidiChannels(const MidiChannels midiChannels) noexcept { _descriptor.midiChannels = midiChannels; }


    /** @brief Initialize the backend audio driver */
    static void InitDriver(void);

    /** @brief Release the backend audio driver */
    static void ReleaseDriver(void);

    /** @brief Get all driver descriptors */
    static DriverDescriptors GetDriverDescriptors(void);
    static void DebugDriverDescriptors(void);

    /** @brief Get all device descriptors */
    static PhysicalDescriptors GetPhysicalDescriptors(void);
    static void DebugPhysicalDescriptors(void);

    /** @brief Reload the device interface according to the internal descriptor */
    void reloadDevice(void);

    /** @brief Reload the audio driver back-end with a specific driver name. Return true on success */
    [[nodiscard]] bool reloadDriver(const Core::TinyString &driverName) noexcept;


private:
    LogicalDescriptor _descriptor {};
    AudioCallback _callback {};
    DeviceID _deviceID {};

    /** @brief Internal audio callback, called by backend */
    static void InternalAudioCallback(void *userdata, std::uint8_t *data, int size) noexcept;
};

static_assert_fit_double_cacheline(Audio::Device);
