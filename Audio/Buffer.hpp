/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
#include <atomic>
#include <array>
#include <memory_resource>

#include "Base.hpp"

namespace Audio
{
    class Buffer;
    class BufferView;

    using BufferViews = Core::TinyVector<BufferView>;

    namespace Internal
    {
        class BufferBase;
        class BufferAllocator;

        /** @brief Header of the buffer allocation */
        struct alignas_cacheline AllocationHeader
        {
            std::size_t bucketIndex {};
            std::size_t capacity {};
            std::size_t size {};
            std::size_t channelByteSize {};
            SampleRate sampleRate {};
            ChannelArrangement channelArrangement {};
            Format format {};
            // Chrono timestamp {}
            AllocationHeader *next { nullptr };
        };
    };
};

class alignas_cacheline Audio::Internal::BufferAllocator
{
public:
    /** @brief Allocaator parameters */
    static constexpr std::size_t MinAllocationPower = 6u;
    static constexpr std::size_t MaxAllocationPower = 30u;
    static constexpr std::size_t OutOfRangeAllocationPower = 1ull << (MaxAllocationPower + 1);
    static constexpr std::size_t AllocationPowerRange = MaxAllocationPower - MinAllocationPower;
    static constexpr std::size_t MinAllocationSize = 1ull << MinAllocationPower;
    static constexpr std::size_t MaxAllocationSize = 1ull << MaxAllocationPower;

    /** @brief Allocates a buffer in memory, setting-up its header */
    [[nodiscard]] static inline AllocationHeader *Allocate(
            const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format) noexcept
        { return _Instance.allocate(channelByteSize, sampleRate, channelArrangement, format); }

    /** @brief Deallocates a buffer in memory */
    static inline void Deallocate(AllocationHeader * const header) noexcept
        { _Instance.deallocate(header); }

    /** @brief Free every retained allocations */
    static void Clear(void) noexcept
        { _Instance.clear(); }

private:
    std::array<std::atomic<AllocationHeader *>, AllocationPowerRange> _buckets {};

    static BufferAllocator _Instance;

    /** @brief Destructor */
    ~BufferAllocator(void) noexcept { clear(); }

    /** @brief Allocates a buffer in memory, setting-up its header */
    [[nodiscard]] AllocationHeader *allocate(
            const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format) noexcept;

    /** @brief Deallocates a buffer in memory */
    void deallocate(AllocationHeader * const header) noexcept;

    /** @brief Release all internal memory */
    void clear(void) noexcept;

    /** @brief Fallback that allocates memory for a buffer */
    [[nodiscard]] static AllocationHeader *AllocateFallback(
            const std::size_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format,
            const std::size_t usedSize, const std::size_t capacity, const std::size_t bucketIndex) noexcept;
};


/** @brief A BufferBase is a helper base class for any Buffer or BufferView */
class alignas_eighth_cacheline Audio::Internal::BufferBase
{
public:
    /** @brief Copy constructor */
    BufferBase(const BufferBase &other) noexcept = default;

    /** @brief Move constructor */
    BufferBase(BufferBase &&other) noexcept : _header(other._header) { other._header = nullptr; }

    /** @brief Destruct the view */
    ~BufferBase(void) noexcept = default;

    /** @brief Copy assignment */
    BufferBase &operator=(const BufferBase &other) noexcept = default;

    /** @brief Move assignment */
    BufferBase &operator=(BufferBase &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    void swap(BufferBase &other) noexcept { std::swap(_header, other._header); }

    /** @brief Clear the internal content */
    void clear(void) { std::memset(reinterpret_cast<void *>(_header + 1), 0, _header->channelByteSize * static_cast<std::size_t>(_header->channelArrangement)); }

    /** @brief Fast allocation check */
    [[nodiscard]] operator bool(void) const noexcept { return _header; }

    /** @brief Get data pointer reintrepreted to a given type & channel */
    template<typename Type>
    [[nodiscard]] Type *data(const Channel channel) noexcept
        { return const_cast<Type *>(std::as_const<BufferBase>(*this).data<Type>(channel)); }

    /** @brief Get constant data pointer reintrepreted to a given type & channel */
    template<typename Type>
    [[nodiscard]] const Type *data(const Channel channel) const noexcept
        { return reinterpret_cast<Type *>(_header + 1) + _header->channelByteSize * static_cast<std::uint32_t>(channel); }

    /** @brief Get data pointer reintrepreted to a given type */
    template<typename Type>
    [[nodiscard]] Type *data(void) noexcept
        { return reinterpret_cast<Type *>(_header + 1); }

    /** @brief Get constant data pointer reintrepreted to a given type */
    template<typename Type>
    [[nodiscard]] const Type *data(void) const noexcept
        { return reinterpret_cast<Type *>(_header + 1); }


    /** @brief Get the byte data pointer */
    [[nodiscard]] std::uint8_t *byteData(void) noexcept { return reinterpret_cast<std::uint8_t *>(_header + 1); }
    [[nodiscard]] const std::uint8_t *byteData(void) const noexcept { return reinterpret_cast<std::uint8_t *>(_header + 1); }


    /** @brief Get the byte size per channel */
    [[nodiscard]] std::size_t channelByteSize(void) const noexcept { return _header->channelByteSize; }

    /** @brief Get the sample count per cahnnel */
    [[nodiscard]] std::size_t channelSampleCount(void) const noexcept { return _header->channelByteSize / GetFormatByteLength(_header->format); }


    /** @brief Get the internal sample rate */
    [[nodiscard]] SampleRate sampleRate(void) const noexcept { return _header->sampleRate; }

    /** @brief Get the channel arrangement of the buffer */
    [[nodiscard]] ChannelArrangement channelArrangement(void) const noexcept { return _header->channelArrangement; }

    /** @brief Get the format of the buffer */
    [[nodiscard]] Format format(void) const noexcept { return _header->format; }


    /** @brief Get the buffer size relative to a given type */
    template<typename Type>
    [[nodiscard]] std::size_t size(void) const noexcept { return _header->size / sizeof(Type); }

    /** @brief Get the buffer capacity relative to a given type */
    template<typename Type = std::byte>
    [[nodiscard]] std::size_t capacity(void) const noexcept { return _header->capacity / sizeof(Type); }

public: // Internal public functions for buffers compatibility
    /** @brief Get the allocation header */
    [[nodiscard]] AllocationHeader *header(void) const noexcept { return _header; }

private:
    Internal::AllocationHeader *_header { nullptr };

protected:
    /** @brief Private constructor */
    BufferBase(AllocationHeader * const header) noexcept
        : _header(header) {}

    /** @brief Header setter, internal uses ! */
    void setHeader(AllocationHeader *newHeader) noexcept { _header = newHeader; }
};

static_assert_fit_eighth_cacheline(Audio::Internal::BufferBase);

/** @brief A Buffer manage ownership of his data */
class alignas_eighth_cacheline Audio::Buffer : public Audio::Internal::BufferBase
{
public:
    /** @brief Default constructor */
    Buffer(void) : BufferBase(nullptr) {}

    /** @brief Allocate the buffer */
    Buffer(const std::uint32_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format)
        : Internal::BufferBase(Internal::BufferAllocator::Allocate(channelByteSize, sampleRate, channelArrangement, format)) {}

    /** @brief A buffer cannot be copied, use a BufferView or 'copy' function instead */
    Buffer(const Buffer &other) = delete;

    /** @brief Move constructor */
    Buffer(Buffer &&other) noexcept = default;

    /** @brief Delete the allocated buffer */
    ~Buffer(void) noexcept { release(); }

    /** @brief Move assignment */
    Buffer &operator=(Buffer &&other) noexcept = default;

    /** @brief Release buffer memory */
    void release(void) noexcept { if (header()) Internal::BufferAllocator::Deallocate(header()); setHeader(nullptr); }


    /** @brief Resize the buffer if needed to fit requirements */
    void resize(const std::uint32_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement channelArrangement, const Format format) noexcept;

    /** @brief Copy the target buffer */
    void copy(const Internal::BufferBase &target);

    /** @brief Copy a range of the target (in bytes) */
    void copyRange(const Internal::BufferBase &target, const std::size_t from, const std::size_t to);


    /** @brief Resample the actual buffer, resize if needed */
    template<typename Type>
    void resample(const SampleRate newSampleRate) noexcept;
};

static_assert_fit_eighth_cacheline(Audio::Buffer);

/** @brief A BufferView holds a reference to an existing buffer without managing data ownership */
class Audio::BufferView : public Internal::BufferBase
{
public:
    /** @brief Construct the view using an existing buffer */
    BufferView(Buffer &source) noexcept
        : Internal::BufferBase(source.header()) {}

    /** @brief Copy constructor */
    BufferView(const BufferView &other) noexcept = default;

    /** @brief Move constructor */
    BufferView(BufferView &&other) noexcept = default;

    /** @brief Destruct the view */
    ~BufferView(void) noexcept = default;

    /** @brief Copy assignment */
    BufferView &operator=(const BufferView &other) noexcept = default;

    /** @brief Move assignment */
    BufferView &operator=(BufferView &&other) noexcept = default;
};

static_assert_fit_eighth_cacheline(Audio::BufferView);

#include "Buffer.ipp"
