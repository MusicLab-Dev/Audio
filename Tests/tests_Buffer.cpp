/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Buffer class
 */

#include <gtest/gtest.h>

#include <Audio/Buffer.hpp>

using namespace Audio;


static void AssertAllocationHeader(const Internal::AllocationHeader *header,
        const std::size_t bucketIndex, const std::size_t capacity, const std::size_t size,
        const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format)
{
    ASSERT_TRUE(header);
    ASSERT_EQ(header->bucketIndex, bucketIndex);
    ASSERT_EQ(header->capacity, capacity);
    ASSERT_EQ(header->size, size);
    ASSERT_EQ(header->channelByteSize, channelByteSize);
    ASSERT_EQ(header->sampleRate, sampleRate);
    ASSERT_EQ(header->channelArrangement, channelArrangement);
    ASSERT_EQ(header->format, format);
}

TEST(Buffer, BufferAllocator)
{
    auto header = Internal::BufferAllocator::Allocate(1, 1, ChannelArrangement::Mono, Format::Fixed8);
    AssertAllocationHeader(header, 0, Core::CacheLineSize, 1, 1, 1, ChannelArrangement::Mono, Format::Fixed8);
    Internal::BufferAllocator::Deallocate(header);

    // for (auto i = 0; i < Internal::BufferAllocator::MinAllocationSize; ++i) {
    //     header = Internal::BufferAllocator::Allocate(i, 1, ChannelArrangement::Mono, Format::Fixed8);
    //     AssertAllocationHeader(header, 0, Core::CacheLineSize, i, i, 1, ChannelArrangement::Mono, Format::Fixed8);
    //     Internal::BufferAllocator::Deallocate(header);
    // }

    // std::cout << "================\n";

    for (auto i = Internal::BufferAllocator::MinAllocationPower; i < Internal::BufferAllocator::MaxAllocationPower; ++i) {
        // std::cout << "::::::::::pow: " << i << std::endl;
        header = Internal::BufferAllocator::Allocate(1ull << i, 1, ChannelArrangement::Mono, Format::Fixed8);
        AssertAllocationHeader(header, i - Internal::BufferAllocator::MinAllocationPower, 1ull << i, 1ull << i, 1ull << i, 1, ChannelArrangement::Mono, Format::Fixed8);
        Internal::BufferAllocator::Deallocate(header);
        // break;
    }
}

TEST(Buffer, BufferAllocatorSmallestSize)
{
    Internal::BufferAllocator::Clear();

    const auto header = Internal::BufferAllocator::Allocate(1, 2, ChannelArrangement::Mono, Format::Fixed8);
    AssertAllocationHeader(header, 0, Core::CacheLineSize, 1, 1, 2, ChannelArrangement::Mono, Format::Fixed8);
    Internal::BufferAllocator::Deallocate(header);

    const auto newHeader = Internal::BufferAllocator::Allocate(Core::CacheLineSize / 2, 3, ChannelArrangement::Stereo, Format::Fixed8);
    ASSERT_EQ(newHeader, header);
    AssertAllocationHeader(newHeader, 0, Core::CacheLineSize, Core::CacheLineSize, Core::CacheLineSize / 2, 3, ChannelArrangement::Stereo, Format::Fixed8);
    Internal::BufferAllocator::Deallocate(newHeader);
}

TEST(Buffer, BufferAllocatorOutOfRange)
{
    constexpr auto UsedSize = Internal::BufferAllocator::MaxAllocationSize + 1;

    Internal::BufferAllocator::Clear();

    const auto header = Internal::BufferAllocator::Allocate(UsedSize, 1, ChannelArrangement::Mono, Format::Fixed8);
    AssertAllocationHeader(header, Internal::BufferAllocator::OutOfRangeAllocationPower, UsedSize, UsedSize, UsedSize, 1, ChannelArrangement::Mono, Format::Fixed8);
    Internal::BufferAllocator::Deallocate(header);
}


TEST(Buffer, BufferSmallCopy)
{
    Internal::BufferAllocator::Clear();

    Buffer source(4, 0, ChannelArrangement::Stereo, Format::Fixed8);
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

TEST(Buffer, BufferGrow)
{
    Buffer source(10u, 44100u, ChannelArrangement::Stereo, Format::Fixed8);
    std::memset(source.byteData(), 42, source.size<std::uint8_t>());
    const auto size = source.size<std::uint8_t>();
    for (auto i = 0u; i < size; ++i)
        ASSERT_EQ(source.byteData()[i], 42);
    source.grow(20u);
    std::memset(source.byteData() + size, 24, size);
    const auto newSize = source.size<std::uint8_t>();
    for (auto i = 0u; i < size; ++i)
        ASSERT_EQ(source.byteData()[i], 42);
    for (auto i = size; i < newSize; ++i)
        ASSERT_EQ(source.byteData()[i], 24);
}