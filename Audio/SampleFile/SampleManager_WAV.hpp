/**
 * @ Author: Pierre Veysseyre
 * @ Description: SampleManager_WAV.hpp
 */

#pragma once

#include <utility>

#include <Audio/Buffer.hpp>
#include "SampleManagerSpecs.hpp"

namespace Audio
{
    struct SampleManager_WAV;
};

struct Audio::SampleManager_WAV
{
    static constexpr const char * const Extension = ".wav";

    // http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
    // https://wavefilegem.com/how_wave_files_work.html

    struct HeaderChunk {
        char riff[4]; // "RIFF"
        std::uint32_t fileSize { 0u }; // (in bytes) fileSize - 8
        char wave[4]; // "WAVE"
    };

    struct HeaderFmt {
        char fmt[4]; //"fmt "
        std::uint32_t blockSize { 0u }; // 16 (0x10)

        std::uint16_t audioFormat { 0u }; // 1: PCM, 3: IEEE float
        std::uint16_t channelArrangement { 0u };
        std::uint32_t sampleRate { 0u };
        std::uint32_t bytePerSec { 0u }; // sampleRate * BytePerBlock
        std::uint16_t bytePerBlock { 0u }; // channelArrangement * BitsPerSample / 8
        std::uint16_t bitsPerSample { 0u }; // 8: char, 16: short, 32: int
    };

    struct HeaderNonPCM {
        std::uint16_t extensionSize { 0u }; // 0 or 22
        std::uint16_t validBitsPerSample { 0u };
        std::uint32_t channelMask { 0u };
        std::uint64_t subFormat_0 { 0u };
        std::uint64_t subFormat_1 { 0u };
    };

    struct HeaderFact {
        char fact[4]; // "fact"
        std::uint32_t factSize { 0u }; // minimum 4
        std::uint32_t samplePerChannel { 0u };
    };

    struct HeaderData {
        char data[4]; // "data"
        std::uint32_t dataSize { 0u }; // (in bytes) fileSize - 44
    };

    /** @brief Load a audio WAV file into a buffer */
    static Buffer LoadFile(const std::string &path, SampleSpecs &specs);

    static void WriteFile(const std::string &path, const BufferView &inputBuffer) {}
};

#include "SampleManager_WAV.ipp"