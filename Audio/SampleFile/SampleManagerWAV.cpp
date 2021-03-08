/**
 * @ Author: Pierre Veysseyre
 * @ Description: SampleManagerWAV.cpp
 */

#include <fstream>

#include "SampleManagerWAV.hpp"

#include <iostream>

using namespace Audio;

Buffer SampleManagerWAV::LoadFile(const std::string &path, SampleSpecs &specs)
{
    auto realPath = std::filesystem::path(path);

    std::cout << "exists() = " << std::filesystem::exists(realPath) << "\n"
        << "root_name() = " << realPath.root_name() << "\n"
        << "root_path() = " << realPath.root_path() << "\n"
        << "relative_path() = " << realPath.relative_path() << "\n"
        << "parent_path() = " << realPath.parent_path() << "\n"
        << "filename() = " << realPath.filename() << "\n"
        << "stem() = " << realPath.stem() << "\n"
        << "extension() = " << realPath.extension() << "\n";
    if (!std::filesystem::exists(realPath))
        throw std::runtime_error("Wav file path doesn't exist !");

    std::ifstream file;
    file.open(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("Wav file doesn't open !");

    HeaderChunk chunk;
    file.read(reinterpret_cast<char *>(&chunk), sizeof(chunk));
    std::cout << "riff: '" << chunk.riff[0] << chunk.riff[1] << chunk.riff[2] << chunk.riff[3] << "'" << std::endl;
    std::cout << "fileSize: " << chunk.fileSize << std::endl;
    std::cout << "wave: '" << chunk.wave[0] << chunk.wave[1] << chunk.wave[2] << chunk.wave[3] << "'" << std::endl;

    HeaderFmt fmt;
    file.read(reinterpret_cast<char *>(&fmt), sizeof(fmt));
    std::cout << "fmt: '" << fmt.fmt[0] << fmt.fmt[1] << fmt.fmt[2] << fmt.fmt[3] << "'" << std::endl;
    std::cout << "blockSize: " << fmt.blockSize << std::endl;
    std::cout << "audioFormat: " << fmt.audioFormat << std::endl;
    std::cout << "channelArrangement: " << fmt.channelArrangement << std::endl;
    std::cout << "sampleRate: " << fmt.sampleRate << std::endl;
    std::cout << "bytePerSec: " << fmt.bytePerSec << std::endl;
    std::cout << "bytePerBlock: " << fmt.bytePerBlock << std::endl;
    std::cout << "bitsPerSample: " << fmt.bitsPerSample << std::endl;

    if (fmt.audioFormat != 1) {
        HeaderFact fact;
        file.read(reinterpret_cast<char *>(&fact), sizeof(fact));
        std::cout << "fact: '" << fact.fact[0] << fact.fact[1] << fact.fact[2] << fact.fact[3] << "'" << std::endl;
        std::cout << "factSize: " << fact.factSize << std::endl;

        HeaderNonPCM nonPCM;
        file.read(reinterpret_cast<char *>(&nonPCM), sizeof(nonPCM));
        std::cout << "extensionSize: " << nonPCM.extensionSize << std::endl;
        std::cout << "validBitsPerSample: " << nonPCM.validBitsPerSample << std::endl;
        std::cout << "channelMask: " << nonPCM.channelMask << std::endl;
        std::cout << "subFormat_0: " << nonPCM.subFormat_0 << std::endl;
        std::cout << "subFormat_1: " << nonPCM.subFormat_1 << std::endl;
    }

    HeaderData data;
    file.read(reinterpret_cast<char *>(&data), sizeof(data));
    std::cout << "data: '" << data.data[0] << data.data[1] << data.data[2] << data.data[3] << "'" << std::endl;
    std::cout << "dataSize: " << data.dataSize << std::endl;

    const auto channelByteSize = data.dataSize / fmt.channelArrangement;

    constexpr std::uint16_t WavIntFlag = 1u;
    constexpr std::uint16_t WavFloatFlag = 3u;
    constexpr std::uint16_t WavALawFlag = 6u; // Specific cases
    constexpr std::uint16_t WavMuLawFlag = 7u; // Specific cases

    /** @brief Get the sample format from a sample bit size */
    constexpr auto GetSampleFormat = [](const HeaderFmt &fmtHeader) -> Format {
        constexpr std::size_t BitsPerByte = 8;
        if (fmtHeader.audioFormat == WavFloatFlag)
            return Format::Floating32;
        else if (fmtHeader.audioFormat != WavIntFlag)
            return Format::Unknown;
        switch (fmtHeader.bitsPerSample) {
        case sizeof(std::int8_t) * BitsPerByte:
            return Format::Fixed8;
        case sizeof(std::int16_t) * BitsPerByte:
            return Format::Fixed16;
        case sizeof(std::int32_t) * BitsPerByte:
            return Format::Fixed32;
        default:
            return Format::Unknown;
        }
    };

    Format sampleFormat = GetSampleFormat(fmt);
    if (sampleFormat == Format::Unknown)
        throw std::logic_error("SampleManagerWAV::LoadFile: Unsupported audio format.");
    Buffer outBuffer(channelByteSize, fmt.sampleRate, static_cast<ChannelArrangement>(fmt.channelArrangement), sampleFormat);
    std::vector<char> buf(data.dataSize);
    file.read(buf.data(), data.dataSize);

    std::cout << "channelByteSize: " << channelByteSize << std::endl;

    for (auto iChannel = 0u; iChannel < fmt.channelArrangement; ++iChannel) {
        // CHANGE THIS !
        for (auto i = 0u; i < channelByteSize / (fmt.bitsPerSample / 8); ++i) {
            // std::cout << iChannel << " - " << i << " = " << i * fmt.channelArrangement << std::endl;
            outBuffer.data<float>(static_cast<Channel>(iChannel))[i] = buf[i * fmt.channelArrangement];
        }
    }

    specs.format = fmt.audioFormat;
    specs.channels = fmt.channelArrangement;
    specs.sampleRate = fmt.sampleRate;
    specs.bytePerSample = fmt.bitsPerSample / 8;
    return outBuffer;
}