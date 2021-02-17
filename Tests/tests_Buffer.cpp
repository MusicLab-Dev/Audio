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

TEST(Buffer, BufferAllocator)
{
    auto header = Internal::BufferAllocator::Allocate(1, 1, ChannelArrangement::Mono);
    AssertAllocationHeader(header, 0, Core::CacheLineSize, 1, 1, 1, ChannelArrangement::Mono);
    Internal::BufferAllocator::Deallocate(header);

    // for (auto i = 0; i < Internal::BufferAllocator::MinAllocationSize; ++i) {
    //     header = Internal::BufferAllocator::Allocate(i, 1, ChannelArrangement::Mono);
    //     AssertAllocationHeader(header, 0, Core::CacheLineSize, i, i, 1, ChannelArrangement::Mono);
    //     Internal::BufferAllocator::Deallocate(header);
    // }

    std::cout << "================\n";

    for (auto i = Internal::BufferAllocator::MinAllocationPower; i < Internal::BufferAllocator::MaxAllocationPower; ++i) {
        std::cout << "::::::::::pow: " << i << std::endl;
        header = Internal::BufferAllocator::Allocate(1ull << i, 1, ChannelArrangement::Mono);
        AssertAllocationHeader(header, i - Internal::BufferAllocator::MinAllocationPower, 1ull << i, 1ull << i, 1ull << i, 1, ChannelArrangement::Mono);
        Internal::BufferAllocator::Deallocate(header);
        // break;
    }
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
    Internal::BufferAllocator::Clear();

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

template<typename Type>
static void Foo(Type *buffer, const std::size_t len, std::size_t ratio, bool upScale) {
    ratio += 1;
    if (upScale) {
        for (auto i = 0u; i < len; ++i) {
            // std::cout << ":::" << ratio * i << " -> " << i << std::endl;
            buffer[i] = buffer[ratio * i];
        }
    } else {
        Type tmp[len];
        for (auto i = 0; i < len; ++i)
            tmp[i] = buffer[i];
        const auto outSize = len + (len - 1) * (ratio - 1);
        for (auto i = 0u; i < len; ++i) {
            const auto outIdx = (outSize - (i * ratio) - 1);
            const auto inIdx = (len - i - 1);
            // std::cout << "-" << outIdx << std::endl;
            // std::cout << "--" << inIdx << std::endl;
            // buffer[outIdx] = buffer[inIdx];
            buffer[i * ratio] = tmp[i];
            for (auto k = 0; k < ratio - 1; ++k)
                buffer[i * ratio + k + 1] = 0;
                // buffer[outIdx + k + 1] = 0;
        }
        // for (auto i = 0u; i < len; ++i) {
        //     const auto outIdx = (outSize - (i * ratio) - 1);
        //     const auto inIdx = (len - i - 1);
        //     for (auto k = 0; k < ratio - 1; ++k) {
        //         buffer[outIdx + k + 1] = 0;
        //         // std::cout << "::" << outIdx + k + 1 << std::endl;
        //     }
        // }
    }
}


constexpr auto SizeElem = 16;
constexpr auto Size = sizeof(int) * SizeElem;
constexpr auto Rate = 2u;

TEST(Buffer, ResampleDown)
{
    Internal::BufferAllocator::Clear();

    Buffer source(Size, 44100, ChannelArrangement::Mono);
    AssertAllocationHeader(source.header(), 0, Size, Size, Size, 44100, ChannelArrangement::Mono);
    Buffer output(Size * (Rate + 1), 44100, ChannelArrangement::Mono);
    AssertAllocationHeader(output.header(), 2, Size * 4, Size * (Rate + 1), Size * (Rate + 1), 44100, ChannelArrangement::Mono);

    for (auto i = 0; i < SizeElem; ++i) {
        source.data<int>(Channel::Mono)[i] = i + 1;
        output.data<int>(Channel::Mono)[i] = i + 1;
        // std::cout << "::" << i << std::endl;
    }
    for (auto i = 0; i < SizeElem * Rate; ++i) {
        // std::cout << ":::" << i + SizeElem << std::endl;
        output.data<int>(Channel::Mono)[i + SizeElem] = 0;
    }

    Foo(output.data<int>(Channel::Mono), SizeElem, Rate, false);
    for (auto i = 0u; i < SizeElem; ++i) {
        for (auto k = 0u; k < Rate + 1; ++k) {
            // std::cout << ">" <<i * (Rate + 1) + k << std::endl;
            // std::cout << output.data<int>(Channel::Mono)[i * (Rate + 1) + k] << std::endl;
            if (!((i * (Rate + 1) + k) % (Rate + 1))) {
                EXPECT_EQ(output.data<int>(Channel::Mono)[i * (Rate + 1) + k], i + 1);
            } else {
                EXPECT_EQ(output.data<int>(Channel::Mono)[i * (Rate + 1) + k], 0);
            }
        }
    }

    // source.resample<int>(48000);

}
