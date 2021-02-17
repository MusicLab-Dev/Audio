/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer implementation
 */

#include "DSP/Resampler.hpp"

inline Audio::Internal::AllocationHeader *Audio::Internal::BufferAllocator::AllocateFallback(
        const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement,
        const std::size_t usedSize, const std::size_t capacity, const std::size_t bucketIndex) noexcept
{
    // std::cout << "AllocateFallback: " << channelByteSize << ", " << usedSize << ", " << capacity << ", " << bucketIndex << std::endl;
    if (const auto data = Core::Utils::AlignedAlloc<Core::CacheLineSize>(capacity + Core::CacheLineSize); data) {
        return new (data) AllocationHeader {
            /* bucketIndex: */ bucketIndex,
            /* capacity: */ capacity,
            /* size: */ usedSize,
            /* channelByteSize: */ channelByteSize,
            /* sampleRate: */ sampleRate,
            /* channelArrangement: */ channelArrangement
        };
    } else
        return nullptr;
}

inline void Audio::Buffer::resize(const std::uint32_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement) noexcept
{
    const auto totalSize = channelByteSize * static_cast<std::size_t>(channelArrangement);

    if (*this && totalSize <= capacity()) {
        header()->size = totalSize;
        header()->channelByteSize = channelByteSize;
        header()->sampleRate = sampleRate;
        header()->channelArrangement = channelArrangement;
    } else {
        *this = Buffer(channelByteSize, sampleRate, channelArrangement);
    }
}

inline void Audio::Buffer::copy(const Internal::BufferBase &target)
{
    if (*this && target.size<std::byte>() <= capacity()) {
        header()->size = target.header()->size;
        header()->channelByteSize = target.header()->channelByteSize;
        header()->sampleRate = target.header()->sampleRate;
        header()->channelArrangement = target.header()->channelArrangement;
        std::memcpy(byteData(), target.byteData(), target.size<std::byte>());
    } else {
        *this = Buffer(target.channelByteSize(), target.sampleRate(), target.channelArrangement());
        std::memcpy(byteData(), target.byteData(), target.size<std::byte>());
    }
}

#include <iostream>

template<typename Type>
inline void Audio::Buffer::resample(const SampleRate newSampleRate) noexcept
{
    if (!newSampleRate || newSampleRate == sampleRate())
        return;
    const auto newSize = DSP::Resampler<Type>::GetResamplingSizeSampleRate(size<Type>(), sampleRate(), newSampleRate) * static_cast<std::size_t>(channelArrangement());
    std::cout << "size: " << channelByteSize() << std::endl;
    std::cout << "new size: " << newSize << std::endl;
    std::cout << "capacity: " << capacity<Type>() << std::endl;
    // Check if the resampled buffer fit in the actual one
    if (newSampleRate > sampleRate() && (capacity<Type>() < newSize)) {
        std::cout << "RESIZE\n";
        resize(newSize * sizeof(Type), newSampleRate, channelArrangement());
    }
    // return DSP::Resampler<Type>::ResampleOctave(data<Type>)
}
