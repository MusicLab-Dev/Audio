/**
 * @ Author: Pierre Veysseyre
 * @ Description: SampleLoader.hpp
 */

#pragma once

#include <Audio/Buffer.hpp>

namespace Audio
{
    struct SampleLoader;
};

struct Audio::SampleLoader
{
    // http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

    struct HeaderChunk_WAV {
        char riff[4]; // "RIFF"
        std::uint32_t fileSize { 0u }; // (in bytes) fileSize - 8
        char wave[4]; // "WAVE"
    };

    struct HeaderFmt_WAV {
        char fmt[4]; //"fmt "
        std::uint32_t blockSize { 0u }; // 16 (0x10)

        std::uint16_t audioFormat { 0u }; // 1: PCM
        std::uint16_t channelArrangement { 0u };
        std::uint32_t sampleRate { 0u };
        std::uint32_t bytePerSec { 0u }; // sampleRate * BytePerBlock
        std::uint16_t bytePerBlock { 0u }; // channelArrangement * BitsPerSample / 8
        std::uint16_t bitsPerSample { 0u };
    };

    struct HeaderNonPCM_WAV {
        std::uint16_t extensionSize { 0u }; // 0 or 22
        std::uint16_t validBitsPerSample { 0u };
        std::uint32_t channelMask { 0u };
        std::uint64_t subFormat_0 { 0u };
        std::uint64_t subFormat_1 { 0u };
    };

    struct HeaderFact_WAV {
        char fact[4]; // "fact"
        std::uint32_t factSize { 0u }; // minimum 4
        std::uint32_t samplePerChannel { 0u };
    };

    struct HeaderData_WAV {
        char data[4]; // "data"
        std::uint32_t dataSize { 0u }; // (in bytes) fileSize - 44
    };



    static Buffer LoadWAV(const std::string &path);
};
