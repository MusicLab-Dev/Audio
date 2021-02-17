/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer
 */

#include <bitset>

#include "Buffer.hpp"

using namespace Audio;

Internal::BufferAllocator Internal::BufferAllocator::_Instance {};


Internal::AllocationHeader *Internal::BufferAllocator::allocate(
        const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement) noexcept
{
    const std::size_t usedSize = channelByteSize * static_cast<std::size_t>(channelArrangement);
    std::size_t capacity = usedSize;
    std::size_t bucketIndex = 0u;

    // Check if the capacity is out of range
    if (capacity > MaxAllocationSize)
        return AllocateFallback(channelByteSize, sampleRate, channelArrangement, usedSize, capacity, OutOfRangeAllocationPower);
    // Else, determines the real bucket capacity and index
    else {
        if (capacity <= MinAllocationSize)
            capacity = MinAllocationSize;
        else {
            const std::bitset<64> capacityBit(capacity);
            capacity >>= (MinAllocationPower + 1);
            while (capacity != 0) {
                capacity >>= 1;
                ++bucketIndex;
            }
            if (capacityBit.count() > 1) {
                capacity = (1ull << (bucketIndex + MinAllocationPower + 1));
                bucketIndex++;
            } else {
                capacity = (1ull << (bucketIndex + MinAllocationPower));
            }
        }
    }
        // std::cout << "final alloc: " << capacity << std::endl;

    // Search if a bucket is available for a given size
    auto &bucket = _buckets.at(bucketIndex);
    AllocationHeader *expected, *desired;
    while (true) {
        expected = bucket.load();
        if (!expected)
            return AllocateFallback(channelByteSize, sampleRate, channelArrangement, usedSize, capacity, bucketIndex);
        desired = expected->next;
        if (bucket.compare_exchange_weak(expected, desired))
            break;
    }
    expected->size = usedSize;
    expected->channelByteSize = channelByteSize;
    expected->sampleRate = sampleRate;
    expected->channelArrangement = channelArrangement;
    expected->next = nullptr;
    return expected;
}

void Internal::BufferAllocator::deallocate(AllocationHeader * const header) noexcept
{
    if (header->bucketIndex == OutOfRangeAllocationPower) {
        Core::Utils::AlignedFree(header);
        return;
    }
    auto &bucket = _buckets.at(header->bucketIndex);
    AllocationHeader *expected;
    while (true) {
        expected = bucket.load();
        if (expected)
            header->next = expected;
        else
            header->next = nullptr;
        if (bucket.compare_exchange_weak(expected, header))
            break;
    }
}

void Internal::BufferAllocator::clear(void) noexcept
{
    for (auto &bucket : _buckets) {
        AllocationHeader *expected, *desired;
        while (true) {
            expected = bucket.load();
            if (!expected)
                break;
            desired = expected->next;
            if (!bucket.compare_exchange_weak(expected, desired))
                continue;
            Core::Utils::AlignedFree(expected);
            if (desired == nullptr)
                break;
        }
    }
}