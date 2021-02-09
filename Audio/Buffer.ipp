/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer implementation
 */

inline Audio::Internal::AllocationHeader *Audio::Internal::BufferAllocator::AllocateFallback(
        const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement,
        const std::size_t usedSize, const std::size_t capacity, const std::size_t bucketIndex) noexcept
{
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
