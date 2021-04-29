/**
 * @ Author: Pierre Veysseyre
 * @ Description: SampleManagerWAV.cpp
 */

#include <fstream>

#include <SDL2/SDL.h>

#include "SampleManagerWAV.hpp"

#include <iostream>

using namespace Audio;

Buffer SampleManagerWAV::LoadFile(const std::string &path, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs)
{
    UNUSED(displaySpecs);
    UNUSED(desiredSpecs);

    SDL_AudioSpec sdlFileSpecs {};
    std::uint8_t *sdlBuffer { nullptr };
    std::uint32_t sdlBufferByteSize { 0u };

    if (!SDL_LoadWAV(path.c_str(), &sdlFileSpecs, &sdlBuffer, &sdlBufferByteSize)) {
        std::cout << SDL_GetError() << std::endl;
        throw std::runtime_error("SampleManagerWAV::LoadFile: failed to load wave file: " + std::string(SDL_GetError()));
    }

    constexpr auto GetFormatFromSDL = [](const SDL_AudioFormat sdlFormat) -> Format {
        switch (sdlFormat) {
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

    constexpr auto GetSDLFormat = [](const Format format) -> SDL_AudioFormat {
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

    std::size_t channelByteSize { static_cast<std::size_t>(sdlBufferByteSize) / static_cast<std::size_t>(sdlFileSpecs.channels) };
    Format format { GetFormatFromSDL(sdlFileSpecs.format) };
    ChannelArrangement channelArrangement { static_cast<ChannelArrangement>(sdlFileSpecs.channels) };
    SampleRate sampleRate { static_cast<SampleRate>(sdlFileSpecs.freq) };
    Buffer fileBuffer(channelByteSize, sampleRate, channelArrangement, format);

    fileSpecs.channelArrangement = channelArrangement;
    fileSpecs.sampleRate = sampleRate;
    fileSpecs.format = format;
    fileSpecs.channelByteSize = channelByteSize;

    // Convert to desired specs !
    if ((desiredSpecs.sampleRate != fileSpecs.sampleRate) || (desiredSpecs.format != fileSpecs.format) || (desiredSpecs.channelArrangement != fileSpecs.channelArrangement)) {
        // Setup SDL converter
        std::cout << "SampleManagerWAV::LoadFile: CONVERTING" << std::endl;
        SDL_AudioCVT sdlConverter {};
        if (SDL_BuildAudioCVT(
            &sdlConverter,
            sdlFileSpecs.format, sdlFileSpecs.channels, sdlFileSpecs.freq,
            GetSDLFormat(desiredSpecs.format), static_cast<std::uint8_t>(desiredSpecs.channelArrangement), static_cast<int>(desiredSpecs.sampleRate)
        ) == -1) {
            std::cout << SDL_GetError() << std::endl;
            throw std::runtime_error("SampleManagerWAV::LoadFile: failed to convert audio file with desired specs: " + std::string(SDL_GetError()));
        }
        /** @todo Hmm, maybe remove this SDL_assert with a 'real' assertion ! */
        SDL_assert(sdlConverter.needed);
        sdlConverter.len = sdlBufferByteSize;
        sdlConverter.buf = static_cast<std::uint8_t *>(std::malloc(sdlBufferByteSize * sdlConverter.len_mult));
        std::memcpy(sdlConverter.buf, sdlBuffer, sdlBufferByteSize);
        // Convert the audio
        SDL_ConvertAudio(&sdlConverter);
        fileBuffer.resize(static_cast<std::size_t>(sdlConverter.len_cvt) / static_cast<std::size_t>(desiredSpecs.channelArrangement), desiredSpecs.sampleRate, desiredSpecs.channelArrangement, desiredSpecs.format);
        std::memcpy(fileBuffer.data<std::uint8_t>(), sdlConverter.buf, desiredSpecs.channelByteSize * static_cast<std::size_t>(desiredSpecs.channelArrangement));

    } else {
        std::memcpy(fileBuffer.data<std::uint8_t>(), sdlBuffer, sdlBufferByteSize);
    }

    SDL_FreeWAV(sdlBuffer);
    return fileBuffer;
}

Buffer SampleManagerWAV::LoadFile_old(const std::string &path, const SampleSpecs &desiredSpecs, SampleSpecs &fileSpecs, bool displaySpecs)
{
    UNUSED(path);
    UNUSED(desiredSpecs);
    UNUSED(fileSpecs);
    UNUSED(displaySpecs);

    return Buffer();
    // auto realPath = std::filesystem::path(path);

    // if (displaySpecs) {
    //     std::cout << "exists() = " << std::filesystem::exists(realPath) << "\n"
    //         << "root_name() = " << realPath.root_name() << "\n"
    //         << "root_path() = " << realPath.root_path() << "\n"
    //         << "relative_path() = " << realPath.relative_path() << "\n"
    //         << "parent_path() = " << realPath.parent_path() << "\n"
    //         << "filename() = " << realPath.filename() << "\n"
    //         << "stem() = " << realPath.stem() << "\n"
    //         << "extension() = " << realPath.extension() << "\n";
    // }
    // if (!std::filesystem::exists(realPath))
    //     throw std::runtime_error("SampleManagerWAV::LoadFile: File doesn't exist: " + path);

    // std::ifstream file;
    // file.open(path, std::ios::binary);
    // if (!file.is_open())
    //     throw std::runtime_error("SampleManagerWAV::LoadFile: Failed opening the file: " + path);

    // HeaderChunk chunk;
    // file.read(reinterpret_cast<char *>(&chunk), sizeof(chunk));
    // if (displaySpecs) {
    //     std::cout << "riff: '" << chunk.riff[0] << chunk.riff[1] << chunk.riff[2] << chunk.riff[3] << "'" << std::endl;
    //     std::cout << "fileSize: " << chunk.fileSize << std::endl;
    //     std::cout << "wave: '" << chunk.wave[0] << chunk.wave[1] << chunk.wave[2] << chunk.wave[3] << "'" << std::endl;
    // }
    // HeaderFmt fmt;
    // file.read(reinterpret_cast<char *>(&fmt), sizeof(fmt));
    // if (displaySpecs) {
    //     std::cout << "fmt: '" << fmt.fmt[0] << fmt.fmt[1] << fmt.fmt[2] << fmt.fmt[3] << "'" << std::endl;
    //     std::cout << "blockSize: " << fmt.blockSize << std::endl;
    //     std::cout << "audioFormat: " << fmt.audioFormat << std::endl;
    //     std::cout << "channelArrangement: " << fmt.channelArrangement << std::endl;
    //     std::cout << "sampleRate: " << fmt.sampleRate << std::endl;
    //     std::cout << "bytePerSec: " << fmt.bytePerSec << std::endl;
    //     std::cout << "bytePerBlock: " << fmt.bytePerBlock << std::endl;
    //     std::cout << "bitsPerSample: " << fmt.bitsPerSample << std::endl;
    // }
    // if (fmt.audioFormat != 1) {
    //     HeaderFact fact;
    //     file.read(reinterpret_cast<char *>(&fact), sizeof(fact));
    //     if (displaySpecs) {
    //         std::cout << "fact: '" << fact.fact[0] << fact.fact[1] << fact.fact[2] << fact.fact[3] << "'" << std::endl;
    //         std::cout << "factSize: " << fact.factSize << std::endl;
    //     }
    //     HeaderNonPCM nonPCM;
    //     file.read(reinterpret_cast<char *>(&nonPCM), sizeof(nonPCM));
    //     if (displaySpecs) {
    //         std::cout << "extensionSize: " << nonPCM.extensionSize << std::endl;
    //         std::cout << "validBitsPerSample: " << nonPCM.validBitsPerSample << std::endl;
    //         std::cout << "channelMask: " << nonPCM.channelMask << std::endl;
    //         std::cout << "subFormat_0: " << nonPCM.subFormat_0 << std::endl;
    //         std::cout << "subFormat_1: " << nonPCM.subFormat_1 << std::endl;
    //     }
    // }

    // HeaderData data;
    // file.read(reinterpret_cast<char *>(&data), sizeof(data));
    // if (displaySpecs) {
    //     std::cout << "data: '" << data.data[0] << data.data[1] << data.data[2] << data.data[3] << "'" << std::endl;
    //     std::cout << "dataSize: " << data.dataSize << std::endl;
    // }
    // const auto channelByteSize = data.dataSize / fmt.channelArrangement;

    // /** @brief Get the sample format from a sample bit size */
    // constexpr auto GetSampleFormat = [](const HeaderFmt &fmtHeader) -> Format {
    //     constexpr std::size_t BitsPerByte = 8;
    //     if (fmtHeader.audioFormat == WavFloatFlag)
    //         return Format::Floating32;
    //     else if (fmtHeader.audioFormat != WavIntFlag)
    //         return Format::Unknown;
    //     switch (fmtHeader.bitsPerSample) {
    //     case sizeof(std::int8_t) * BitsPerByte:
    //         return Format::Fixed8;
    //     case sizeof(std::int16_t) * BitsPerByte:
    //         return Format::Fixed16;
    //     case sizeof(std::int32_t) * BitsPerByte:
    //         return Format::Fixed32;
    //     default:
    //         return Format::Unknown;
    //     }
    // };

    // Format sampleFormat = GetSampleFormat(fmt);
    // if (sampleFormat == Format::Unknown)
    //     throw std::logic_error("SampleManagerWAV::LoadFile: Unsupported audio format.");
    // Buffer outBuffer(channelByteSize, fmt.sampleRate, static_cast<ChannelArrangement>(fmt.channelArrangement), sampleFormat);
    // std::vector<char> audioDataBuffer(data.dataSize);
    // file.read(audioDataBuffer.data(), data.dataSize);

    // const std::size_t sampleSize = channelByteSize * fmt.channelArrangement / (fmt.bitsPerSample / 8);
    // switch (sampleFormat) {
    // case Format::Floating32:
    //     WriteToBufferImpl(reinterpret_cast<const float *>(audioDataBuffer.data()), reinterpret_cast<float *>(outBuffer.byteData()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
    //     break;
    // case Format::Fixed8:
    //     WriteToBufferImpl(reinterpret_cast<const std::int8_t *>(audioDataBuffer.data()), reinterpret_cast<std::int8_t *>(outBuffer.byteData()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
    //     break;
    // case Format::Fixed16:
    //     WriteToBufferImpl(reinterpret_cast<const std::int16_t *>(audioDataBuffer.data()), reinterpret_cast<std::int16_t *>(outBuffer.byteData()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
    //     break;
    // case Format::Fixed32:
    //     WriteToBufferImpl(reinterpret_cast<const std::int32_t *>(audioDataBuffer.data()), reinterpret_cast<std::int32_t *>(outBuffer.byteData()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
    //     break;
    // default:
    //     break;
    // }

    // specs.format = fmt.audioFormat;
    // specs.channels = fmt.channelArrangement;
    // specs.sampleRate = fmt.sampleRate;
    // specs.bytePerSample = fmt.bitsPerSample / 8;
    // return outBuffer;
}

bool SampleManagerWAV::WriteFile(const std::string &path, const BufferView &inputBuffer)
{
    std::cout << path << std::endl;

    std::ofstream file;
    file.open(path, std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("SampleManagerWAV::WriteFile: Failed opening the file !");


    const auto fileSize { inputBuffer.size<std::uint8_t>() - 44u };
    HeaderChunk chunk {
        { 'R', 'I', 'F', 'F' },
        static_cast<std::uint32_t>(fileSize),
        { 'W', 'A', 'V', 'E' }
    };

    /** @brief Get the sample format from a sample bit size */
    constexpr auto GetSampleFormatWAV = [](const Audio::Format format) -> std::uint16_t {
        if (format == Format::Unknown)
            throw std::logic_error("SampleManagerWAV::WriteFile: Unsupported audio format.");
        if (format == Format::Fixed8 ||
            format == Format::Fixed8 ||
            format == Format::Fixed16)
            return WavIntFlag;
        return WavFloatFlag;
    };

    auto channels = static_cast<std::uint16_t>(inputBuffer.channelArrangement());
    auto fmtFormat = GetSampleFormatWAV(inputBuffer.format());
    auto sampleByteSize = GetFormatByteLength(inputBuffer.format());
    file.write(reinterpret_cast<char *>(&chunk), sizeof(chunk));
    HeaderFmt fmt {
        { 'f', 'm', 't', ' ' },
        16u, // PCM data == 16
        fmtFormat,
        channels,
        static_cast<std::uint32_t>(inputBuffer.sampleRate()),
        static_cast<std::uint32_t>(inputBuffer.sampleRate() * sampleByteSize),
        static_cast<std::uint16_t>(sampleByteSize * channels),
        static_cast<std::uint16_t>(sampleByteSize * 8u)
    };
    file.write(reinterpret_cast<char *>(&fmt), sizeof(fmt));
    // auto zero = 0u;
    // auto four = 4u;
    if (inputBuffer.format() == Audio::Format::Floating32) {
        // Extension size
        // file.write(reinterpret_cast<char *>(&zero), 2);
    }
    if (fmt.audioFormat != 1) {
        // file.write("fact", 4);
        // file.write(reinterpret_cast<char *>(&four), 2);
        // HeaderNonPCM nonPCM;
        // file.write(reinterpret_cast<char *>(&nonPCM), sizeof(nonPCM));
        // if (displaySpecs) {
        //     std::cout << "extensionSize: " << nonPCM.extensionSize << std::endl;
        //     std::cout << "validBitsPerSample: " << nonPCM.validBitsPerSample << std::endl;
        //     std::cout << "channelMask: " << nonPCM.channelMask << std::endl;
        //     std::cout << "subFormat_0: " << nonPCM.subFormat_0 << std::endl;
        //     std::cout << "subFormat_1: " << nonPCM.subFormat_1 << std::endl;
        // }
    }

    const auto dataSize = fileSize + 44u;
    HeaderData data {
        { 'd', 'a', 't', 'a' },
        static_cast<std::uint32_t>(dataSize)
    };
    file.write(reinterpret_cast<char *>(&data), sizeof(data));
    const auto channelByteSize = data.dataSize / fmt.channelArrangement;


    std::vector<char> audioDataBuffer(dataSize);
    const std::size_t sampleSize = channelByteSize * fmt.channelArrangement / (fmt.bitsPerSample / 8);
    // std::cout << "input: " << inputBuffer.size<std::uint8_t>() << std::endl;
    // std::cout << "out: " << audioDataBuffer.size() << std::endl;
    // std::cout << "sampleSize: " << sampleSize << std::endl;

    switch (inputBuffer.format()) {
    case Format::Floating32:
        WriteFromBufferImpl(reinterpret_cast<const float *>(inputBuffer.byteData()), reinterpret_cast<float *>(audioDataBuffer.data()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
        break;
    case Format::Fixed8:
        WriteFromBufferImpl(reinterpret_cast<const std::int8_t *>(inputBuffer.byteData()), reinterpret_cast<std::int8_t *>(audioDataBuffer.data()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
        break;
    case Format::Fixed16:
        WriteFromBufferImpl(reinterpret_cast<const std::int16_t *>(inputBuffer.byteData()), reinterpret_cast<std::int16_t *>(audioDataBuffer.data()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
        break;
    case Format::Fixed32:
        WriteFromBufferImpl(reinterpret_cast<const std::int32_t *>(inputBuffer.byteData()), reinterpret_cast<std::int32_t *>(audioDataBuffer.data()), static_cast<ChannelArrangement>(fmt.channelArrangement), sampleSize);
        break;
    default:
        break;
    }

    file.write(reinterpret_cast<char *>(audioDataBuffer.data()), dataSize);

    return true;
}
