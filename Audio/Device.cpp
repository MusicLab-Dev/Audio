/**
 * @ Author: Pierre Veysseyre
 * @ Description: Device.cpp
 */

#include <stdexcept>

#include "Device.hpp"

using namespace Audio;

Device::Device(const Descriptor &descriptor, AudioCallback &&callback)
    : _descriptor(descriptor)
{
    reloadDriver(std::move(callback));
}

Device::~Device(void)
{
    SDL_CloseAudioDevice(_deviceID);
}
