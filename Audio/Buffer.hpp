/**
 * @ Author: Pierre Veysseyre
 * @ Description: Buffer
 */

#pragma once

#include <cstddef>
#include <cstring>
#include <memory>
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
    };
};

/** @brief A BufferBase is a helper base class for any Buffer or BufferView */
class alignas_quarter_cacheline Audio::Internal::BufferBase
{
public:
    /** @brief Get data pointer reintrepreted to a given type */
    template<typename Type>
    [[nodiscard]] Type *data(const Channel channel) noexcept
        { return reinterpret_cast<Type *>(_data + _channelByteSize * static_cast<std::uint32_t>(channel)); }

    /** @brief Get constant data pointer reintrepreted to a given type */
    template<typename Type>
    [[nodiscard]] const Type *data(const Channel channel) const noexcept
        { return reinterpret_cast<const Type *>(_data + _channelByteSize * static_cast<std::uint32_t>(channel)); }

    /** @brief Get the byte data pointer */
    [[nodiscard]] std::byte *byteData(void) noexcept { return _data; }
    [[nodiscard]] const std::byte *byteData(void) const noexcept { return _data; }


    /** @brief Get the byte size per channel */
    [[nodiscard]] std::uint32_t channelByteSize(void) const noexcept { return _channelByteSize; }

    /** @brief Get the internal sample rate */
    [[nodiscard]] SampleRate sampleRate(void) const noexcept { return _sampleRate; }

    /** @brief Get the buffer size relative to a given type */
    template<typename Type>
    [[nodiscard]] std::uint32_t size(void) const noexcept { return _channelByteSize / sizeof(Type); }

    /** @brief Get the channel arrangement of the buffer */
    [[nodiscard]] ChannelArrangement channelArrangement(void) const noexcept { return static_cast<ChannelArrangement>(_arrangement); }


    /** @brief Copy constructor */
    BufferBase(const BufferBase &other) noexcept = default;

    /** @brief Move constructor */
    BufferBase(BufferBase &&other) noexcept { swap(other); }

    /** @brief Destruct the view */
    ~BufferBase(void) noexcept = default;

    /** @brief Copy assignment */
    BufferBase &operator=(const BufferBase &other) noexcept = default;

    /** @brief Move assignment */
    BufferBase &operator=(BufferBase &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    void swap(BufferBase &other) noexcept;

protected:
    std::byte *_data { nullptr };
    std::uint32_t _channelByteSize { 0 };
    std::uint32_t _sampleRate : 30;
    std::uint32_t _arrangement : 2;

    /** @brief Private constructor */
    BufferBase(std::byte * const data, const std::uint32_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement arrangement) noexcept
        : _data(data), _channelByteSize(channelByteSize), _sampleRate(sampleRate), _arrangement(static_cast<std::uint32_t>(arrangement)) {}
};

static_assert_fit_quarter_cacheline(Audio::Internal::BufferBase);

/** @brief A Buffer manage ownership of his data */
class alignas_quarter_cacheline Audio::Buffer : public Audio::Internal::BufferBase
{
public:
    /** @brief Allocate the buffer */
    Buffer(const std::uint32_t channelByteSize, const SampleRate sampleRate, const ChannelArrangement arrangement)
        : Internal::BufferBase(
            reinterpret_cast<std::byte *>(
                _Allocator.allocate(channelByteSize * static_cast<std::uint32_t>(arrangement), Core::CacheLineSize)
            ),
            channelByteSize,
            sampleRate,
            arrangement
        ) {}

    /** @brief Default constructor */
    Buffer(void) : BufferBase(nullptr, 0, 0, ChannelArrangement::Mono) {}

    /** @brief A buffer cannot be copied, use a BufferView instead */
    Buffer(const Buffer &other) = delete;

    /** @brief Move constructor */
    Buffer(Buffer &&other) noexcept = default;

    /** @brief Delete the allocated buffer */
    ~Buffer(void) noexcept { if (_data) _Allocator.deallocate(_data, _channelByteSize * static_cast<std::uint32_t>(_arrangement), Core::CacheLineSize); }

    /** @brief Move assignment */
    Buffer &operator=(Buffer &&other) noexcept = default;


    /** @brief Deep copy the buffer */
    [[nodiscard]] Buffer deepCopy(void) const noexcept
    {
        Buffer other(channelByteSize(), sampleRate(), channelArrangement());
        // other.
        return other;
    }


    /** @brief Set the sample rate (does NOT change data) */
    void setSampleRate(const SampleRate sampleRate) noexcept
        { _sampleRate = sampleRate; }

private:
    static inline std::pmr::synchronized_pool_resource _Allocator {};
};

/** @brief A BufferView holds a reference to an existing buffer without managing data ownership */
class Audio::BufferView : public Audio::Internal::BufferBase
{
public:
    /** @brief Construct the view using an existing buffer */
    BufferView(Buffer &source) noexcept
        : Internal::BufferBase(
            source.byteData(),
            source.channelByteSize(),
            source.sampleRate(),
            source.channelArrangement()
        ) {}

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

    // /** @brief Merge multiple BufferView in this one */
    // template<typename Type>
    // [[nodiscard]] BufferView &merge(const BufferViews &views) noexcept {
    //     const auto totalCount = views.size();
    //     const auto bufferSize = size<Type>();

    //     for (auto channel = 0u; channel < static_cast<std::uint32_t>(_arrangement); ++channel) {
    //         auto &channelData = data<Type>(Channel(channel));
    //         for (auto i = 0u; i < bufferSize; ++i) {
    //             auto newData = 0;// channelData[i] / totalCount;
    //             for (auto &view : views) {
    //                 newData += view.data<Type>(Channel(channel))[i] / totalCount;
    //             }
    //             channelData[i] = newData;
    //         }
    //     }
    //     return *this;
    // }
};
#include "Buffer.ipp"
