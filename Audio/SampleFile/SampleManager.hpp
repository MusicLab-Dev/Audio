/**
 * @ Author: Pierre V
 * @ Description: SampleManager
 */

#pragma once

#include <filesystem>

#include <Audio/Buffer.hpp>

#include "SampleManager_WAV.hpp"

namespace Audio
{
    template<typename T>
    struct SampleManager;
};

#include <iostream>

template<typename T>
struct Audio::SampleManager
{
    static const constexpr std::tuple<
        const char *,
        Buffer(*)(const std::string &path, SampleSpecs &specs),
        void(*)(const std::string &path, const BufferView &inputBuffer)
    > SupportedExtension[] {
        { SampleManager_WAV::Extension, &SampleManager_WAV::LoadFile, &SampleManager_WAV::WriteFile }
    };

    static Buffer LoadSampleFile(const std::string &path, SampleSpecs &specs);

    static Buffer LoadSampleFileExtension(const std::string &path, const std::string &ext, SampleSpecs &specs);
};

#include "SampleManager.ipp"
