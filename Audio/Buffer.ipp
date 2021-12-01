/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer implementation
 */

#include "DSP/Resampler.hpp"

inline Audio::Internal::AllocationHeader *Audio::Internal::BufferAllocator::AllocateFallback(
        const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format,
        const std::size_t usedSize, const std::size_t capacity, const std::size_t bucketIndex) noexcept
{
    // std::cout << "AllocateFallback: " << channelByteSize << ", " << usedSize << ", " << capacity << ", " << bucketIndex << std::endl;
    if (const auto data = Core::Utils::AlignedAlloc<Core::CacheLineSize>(capacity + Core::CacheLineSize); data) {
        return new (data) AllocationHeader {
            /* capacity: */ capacity,
            /* channelByteSize: */ channelByteSize,
            /* size: */ usedSize,
            /* bucketIndex: */ bucketIndex,
            /* sampleRate: */ sampleRate,
            /* channelArrangement: */ channelArrangement,
            /* format: */ format
        };
    } else
        return nullptr;
}

inline void Audio::Internal::BufferBase::clear(void) noexcept
{
    std::memset(reinterpret_cast<void *>(_header + 1), 0, _header->channelByteSize * static_cast<std::size_t>(_header->channelArrangement));
}

inline bool Audio::Internal::BufferBase::isZero(void) const noexcept
{
    // We can assert that a buffer has at least 3 * sizeof(size_t) because its buffer data is aligned to cacheline size
    // if (!_header || data<std::size_t>()[0] || data<std::size_t>()[_header->size / 2] || data<std::size_t>()[_header->size - sizeof(std::size_t)])
    //     return false;
    if (!_header)
        return true;
    return std::all_of(byteData(), byteData() + _header->size, [](const auto c) {
        return c == 0u;
    });
}

inline void Audio::Buffer::resize(const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format) noexcept
{
    const auto totalSize = channelByteSize * static_cast<std::size_t>(channelArrangement);

    if (*this && totalSize <= capacity()) {
        header()->size = totalSize;
        header()->channelByteSize = channelByteSize;
        header()->sampleRate = sampleRate;
        header()->channelArrangement = channelArrangement;
        header()->format = format;
    } else {
        *this = Buffer(channelByteSize, sampleRate, channelArrangement, format);
    }
}

inline void Audio::Buffer::grow(const std::size_t channelByteSize) noexcept
{
    const auto totalSize = channelByteSize * static_cast<std::size_t>(channelArrangement());

    if (totalSize <= capacity()) {
        header()->size = totalSize;
        header()->channelByteSize = channelByteSize;
    } else {
        Buffer target(channelByteSize, sampleRate(), channelArrangement(), format());
        std::memcpy(target.byteData(), byteData(), size<std::uint8_t>());
        *this = std::move(target);
    }
}

inline void Audio::Buffer::copy(const Internal::BufferBase &target)
{
    if (*this && target.size<std::uint8_t>() <= capacity()) {
        header()->size = target.header()->size;
        header()->channelByteSize = target.header()->channelByteSize;
        header()->sampleRate = target.header()->sampleRate;
        header()->channelArrangement = target.header()->channelArrangement;
        header()->format = target.header()->format;
        std::memcpy(byteData(), target.byteData(), target.size<std::uint8_t>());
    } else {
        release();
        *this = Buffer(target.channelByteSize(), target.sampleRate(), target.channelArrangement(), target.format());
        std::memcpy(byteData(), target.byteData(), target.size<std::uint8_t>());
    }
}

inline void Audio::Buffer::copyRange(const Internal::BufferBase &target, const std::size_t from, const std::size_t to)
{
    const auto newSize = to - from;
    if (*this && newSize <= capacity()) {
        header()->size = static_cast<std::size_t>(target.header()->channelArrangement) * newSize;
        header()->channelByteSize = newSize;
        header()->sampleRate = target.header()->sampleRate;
        header()->channelArrangement = target.header()->channelArrangement;
        header()->format = target.header()->format;
        std::memcpy(byteData(), target.byteData() + from, newSize);
    } else {
        *this = Buffer(newSize, target.sampleRate(), target.channelArrangement(), target.format());
        std::memcpy(byteData(), target.byteData() + from, newSize);
    }
}

template<typename Type>
inline void Audio::Buffer::resample(const SampleRate newSampleRate) noexcept
{
    if (!newSampleRate || newSampleRate == sampleRate())
        return;

    const auto actualSize = size<Type>();
    const auto newSize = DSP::Resampler<Type>::GetResampleSampleRateBufferSize(actualSize, sampleRate(), newSampleRate);
    DSP::Resampler<Type> resampler;

    grow(newSize);
    resampler.resampleSampleRate(data<Type>(), data<Type>(), actualSize, sampleRate(), newSampleRate);
    header()->sampleRate = newSampleRate;
    return;
}

template<typename Type>
inline void Audio::Buffer::updateVolumeCache(void) noexcept
{
    static_assert(std::is_same_v<Type, float>, "Buffer::updateVolumeCache: Only float is implemented");

    BufferVolumeCache cache {
        0.0, 0.0
    };
    const auto count = size<Type>();
    auto it = data<Type>();
    const auto end = it + count;

    while (it != end) {
        const float abs = std::abs(*it);
        if (abs > cache.peak)
            cache.peak = abs;
        cache.rms += abs * abs;
        ++it;
    }
    cache.peak = ConvertSampleToDecibel(cache.peak);
    cache.rms = ConvertSampleToDecibel(std::sqrt(cache.rms / static_cast<Type>(count)));
    header()->volumeCache.store(cache);
}

inline void Audio::Buffer::resetVolumeCache(void) noexcept
{
    header()->volumeCache.store(BufferVolumeCache {});
}
