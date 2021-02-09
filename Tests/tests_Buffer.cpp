/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Buffer class
 */

#include <gtest/gtest.h>

#include <Audio/Buffer.hpp>

using namespace Audio;


static void AssertAllocationHeader(const Internal::AllocationHeader *header,
        const std::size_t bucketIndex, const std::size_t capacity, const std::size_t size,
        const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement)
{
    ASSERT_TRUE(header);
    ASSERT_EQ(header->bucketIndex, bucketIndex);
    ASSERT_EQ(header->capacity, capacity);
    ASSERT_EQ(header->size, size);
    ASSERT_EQ(header->channelByteSize, channelByteSize);
    ASSERT_EQ(header->sampleRate, sampleRate);
    ASSERT_EQ(header->channelArrangement, channelArrangement);
}

TEST(Buffer, BufferAllocatorSmallestSize)
{
    Internal::BufferAllocator::Clear();

    const auto header = Internal::BufferAllocator::Allocate(1, 2, ChannelArrangement::Mono);
    AssertAllocationHeader(header, 0, Core::CacheLineSize, 1, 1, 2, ChannelArrangement::Mono);
    Internal::BufferAllocator::Deallocate(header);

    const auto newHeader = Internal::BufferAllocator::Allocate(Core::CacheLineSize / 2, 3, ChannelArrangement::Stereo);
    ASSERT_EQ(newHeader, header);
    AssertAllocationHeader(newHeader, 0, Core::CacheLineSize, Core::CacheLineSize, Core::CacheLineSize / 2, 3, ChannelArrangement::Stereo);
    Internal::BufferAllocator::Deallocate(newHeader);
}

TEST(Buffer, BufferAllocatorOutOfRange)
{
    constexpr auto UsedSize = Internal::BufferAllocator::MaxAllocationSize + 1;

    Internal::BufferAllocator::Clear();

    const auto header = Internal::BufferAllocator::Allocate(UsedSize, 1, ChannelArrangement::Mono);
    AssertAllocationHeader(header, Internal::BufferAllocator::OutOfRangeAllocationPower, UsedSize, UsedSize, UsedSize, 1, ChannelArrangement::Mono);
    Internal::BufferAllocator::Deallocate(header);
}


TEST(Buffer, BufferSmallCopy)
{
    Buffer source(4, 0, ChannelArrangement::Stereo);
    std::fill_n(source.data<char>(), 8, 42);
    for (auto i = 0; i < 8; ++i)
        ASSERT_EQ(source.data<char>()[i], 42);
    Buffer target;
    target.copy(source);
    for (auto i = 0; i < 8; ++i)
        ASSERT_EQ(target.data<char>()[i], 42);
    auto oldPtr = source.byteData();
    source.copy(target);
    ASSERT_EQ(source.byteData(), oldPtr);
    for (auto i = 0; i < 8; ++i)
        ASSERT_EQ(source.data<char>()[i], 42);
}


static constexpr BlockSize Size = 1024u;
static constexpr auto ChannelNumber = 2u;

using T = int;