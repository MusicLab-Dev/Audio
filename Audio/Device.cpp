/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device.cpp
 */

#include <stdexcept>
#include <iostream>

#include <Core/StringUtils.hpp>

#include "SDL2.hpp"
#include "Device.hpp"

using namespace Audio;


void Device::InitDriver(void)
{
    if (SDL_Init(SDL_INIT_AUDIO))
        throw std::runtime_error(std::string("Couldn't initialize SDL_Audio: ") + SDL_GetError());

    DebugPhysicalDescriptors();
    DebugDriverDescriptors();
}

void Device::ReleaseDriver(void)
{
    // SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
}

void Device::InternalAudioCallback(void *userdata, std::uint8_t *data, int size) noexcept
{
    try {
        reinterpret_cast<Device *>(userdata)->_callback(data, static_cast<std::size_t>(size));
    } catch (const std::exception &e) {
        std::memset(data, 0, size);
        std::cout << "Device::InternalAudioCallback: Exception thrown into the audio callback:\n\t" << e.what() << std::endl;
    }
}

Device::Device(const LogicalDescriptor &descriptor, AudioCallback &&callback)
    : _descriptor(descriptor), _callback(std::move(callback))
{
    reloadDevice();
}

Device::~Device(void)
{
    SDL_CloseAudioDevice(_deviceID);
}

void Device::start(void)
{
    SDL_PauseAudioDevice(_deviceID, false);
    _running = true;
}

void Device::stop(void)
{
    SDL_PauseAudioDevice(_deviceID, true);
    _running = false;
}

bool Device::reloadDriver(const Core::TinyString &driverName) noexcept
{
    return !SDL_AudioInit(driverName.data());
}

void Device::reloadDevice(void)
{
    std::cout << "[Audio Device] Reloading driver..." << std::endl;
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
    constexpr auto ReverseFormat = [](const SDL_AudioFormat format) -> Format {
        switch (format) {
        case AUDIO_F32:
            return Format::Floating32;
        case AUDIO_S32:
            return Format::Fixed32;
        case AUDIO_S16:
            return Format::Fixed16;
        case AUDIO_S8:
            return Format::Fixed8;
        default:
            return Format::Floating32;
        }
    };

    SDL_AudioSpec desiredSpec {};
    desiredSpec.freq = static_cast<std::size_t>(_descriptor.sampleRate);
    desiredSpec.format = GetFormat(_descriptor.format);
    desiredSpec.samples = _descriptor.blockSize;
    desiredSpec.callback = &Device::InternalAudioCallback;
    desiredSpec.userdata = this;
    desiredSpec.channels = static_cast<std::size_t>(_descriptor.channelArrangement);
    SDL_AudioSpec acquiredSpec;

    if (!(_deviceID = SDL_OpenAudioDevice(_descriptor.name.empty() || _descriptor.name == DefaultDeviceName ? nullptr : _descriptor.name.c_str(), _descriptor.isInput, &desiredSpec, &acquiredSpec, 1))) {
        if (!(_deviceID = SDL_OpenAudioDevice(nullptr, _descriptor.isInput, &desiredSpec, &acquiredSpec, 1)))
            throw std::runtime_error(std::string("Couldn't open any audio device: ") + SDL_GetError());
    }

    if (_descriptor.name.empty())
        _descriptor.name = DefaultDeviceName;
    _descriptor.blockSize = acquiredSpec.samples;
    _descriptor.sampleRate = static_cast<SampleRate>(acquiredSpec.freq);
    _descriptor.format = ReverseFormat(acquiredSpec.format);
    _descriptor.channelArrangement = static_cast<ChannelArrangement>(acquiredSpec.channels);
    std::cout << "[Audio Device] Acquired device: " << _descriptor.blockSize << " " << _descriptor.sampleRate << std::endl;
}

void Device::DebugPhysicalDescriptors(void)
{
    const auto Devices = Device::GetPhysicalDescriptors();

    std::cout << "Devices:" << std::endl;
    for (const auto &d : Devices) {
        std::cout << "  - " << d.name << " (in: " << d.hasInput << ", out: " << d.hasOutput << ")" << std::endl;
    }
}

void Device::DebugDriverDescriptors(void)
{
    const auto Drivers = Device::GetDriverDescriptors();

    std::cout << "Drivers:" << std::endl;
    for (const auto &d : Drivers) {
        std::cout << "  - " << d << std::endl;
    }
}

Device::DriverDescriptors Device::GetDriverDescriptors(void)
{
    DriverDescriptors drivers;
    const auto nDeviceInput { SDL_GetNumAudioDrivers() };

    for (auto i = 0; i < nDeviceInput; ++i) {
        drivers.push_back(Core::TinyString(SDL_GetAudioDriver(i)));
    }
    return drivers;
}

Device::PhysicalDescriptors Device::GetPhysicalDescriptors(const bool recordInputs, const bool recordOutputs)
{
    PhysicalDescriptors devices;

    if (recordInputs) {
    const auto nDeviceInput { SDL_GetNumAudioDevices(true) };
        for (auto i = 0; i < nDeviceInput; ++i) {
            devices.push_back(PhysicalDescriptor {
                Core::TinyString(SDL_GetAudioDeviceName(i, true)),
                true,
                false
            });
        }
    }
    if (recordOutputs) {
        const auto nDeviceOutput { SDL_GetNumAudioDevices(false) };
        for (auto i = 0; i < nDeviceOutput; ++i) {
            if (auto it = std::find_if(devices.begin(), devices.end(), [i](const PhysicalDescriptor &desc) -> bool {
                return desc.name == SDL_GetAudioDeviceName(i, false);
            }); it != devices.end()) {
                it->hasOutput = true;
                continue;
            }
            devices.push_back(PhysicalDescriptor {
                Core::TinyString(SDL_GetAudioDeviceName(i, false)),
                false,
                true
            });
        }
    }
    return devices;
}
