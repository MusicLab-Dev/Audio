/**
 * @ Author: Pierre V
 * @ Description: SampleManager
 */

#pragma once

#include <filesystem>

#include <Audio/Buffer.hpp>

#include "SampleManagerWAV.hpp"

namespace Audio
{
    template<typename Type, bool Normalize = false>
    struct SampleManager;
}

#include <iostream>

template<typename Type, bool Normalize>
struct Audio::SampleManager
{
    static const constexpr std::tuple<
        const char *,
        Buffer(*)(const std::string &path, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs),
        bool(*)(const std::string &path, const BufferView &inputBuffer)
    > SupportedExtension[] {
        { SampleManagerWAV::Extension, &SampleManagerWAV::LoadFile, &SampleManagerWAV::WriteFile }
    };

    [[nodiscard]] static Buffer LoadSampleFile(const std::string &path, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs = false);

    [[nodiscard]] static Buffer LoadSampleFileExtension(const std::string &path, const std::string &ext, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs);

    [[nodiscard]] static bool WriteSampleFile(const std::string &path, const BufferView &sample);

    [[nodiscard]] static bool WriteSampleFileExtension(const std::string &path, const BufferView &sample, const std::string &ext);
};

#include "SampleManager.ipp"
