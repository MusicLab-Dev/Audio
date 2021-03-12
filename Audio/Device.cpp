/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device.cpp
 */

#include <stdexcept>

#include "Device.hpp"

using namespace Audio;

Device::Device(const Descriptor &descriptor, AudioCallback &&callback)
    : _descriptor(descriptor), _callback(std::move(callback))
{
    reloadDriver();
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
