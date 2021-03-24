/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device.cpp
 */

#include <stdexcept>

#include "Device.hpp"

using namespace Audio;

Device::Device(const SDLDescriptor &descriptor, AudioCallback &&callback)
    : _descriptor(descriptor), _callback(std::move(callback))
{
    reloadDevice();
}

Device::~Device(void)
{
    SDL_CloseAudioDevice(_deviceID);
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

void Device::DebugDeviceDescriptors(void)
{
    const auto Devices = Device::GetDeviceDescriptors();

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

    for (auto i = 0u; i < nDeviceInput; ++i) {
        drivers.push_back(SDL_GetAudioDriver(i));
    }
    return drivers;
}

Device::DeviceDescriptors Device::GetDeviceDescriptors(void)
{
    DeviceDescriptors devices;
    const auto nDeviceInput { SDL_GetNumAudioDevices(true) };
    std::cout << nDeviceInput << std::endl;
    for (auto i = 0u; i < nDeviceInput; ++i) {
        devices.push_back({
            SDL_GetAudioDeviceName(i, true),
            true,
            false
        });
    }
    const auto nDeviceOutput { SDL_GetNumAudioDevices(false) };
    for (auto i = 0u; i < nDeviceOutput; ++i) {
        if (auto it = std::find_if(devices.begin(), devices.end(), [i](const DeviceDescriptor &desc) -> bool {
            std::string deviceName(SDL_GetAudioDeviceName(i, false));
            return (desc.name == deviceName);
        }); it != devices.end()) {
            it->hasOutput = true;
            continue;
        }
        devices.push_back({
            SDL_GetAudioDeviceName(i, false),
            false,
            true
        });
    }
    return devices;
}
