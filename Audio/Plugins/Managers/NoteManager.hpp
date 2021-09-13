/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager
 */

#pragma once

#include <array>
#include <memory>
#include <utility>

#include <Core/Vector.hpp>

#include <Audio/Note.hpp>
#include <Audio/DSP/EnvelopeGenerator.hpp>

namespace Audio
{
    template<DSP::EnvelopeType EnvelopeType>
    class NoteManager;

    using NoteManagerDefault = NoteManager<DSP::EnvelopeType::ADSR>;

    template<DSP::EnvelopeType EnvelopeType>
    using NoteManagerPtr = std::unique_ptr<NoteManager<EnvelopeType>>;
}

/** @brief Note manager store states of each note */
template<Audio::DSP::EnvelopeType EnvelopeType>
class alignas_double_cacheline Audio::NoteManager
{
public:
    using IndexArray = std::array<std::uint32_t, KeyCount>;
    using ActiveKeyList = Core::TinySmallVector<Key, Core::CacheLineQuarterSize>;
    using KeyArray = Core::TinySmallVector<Key, Core::CacheLineQuarterSize>;

    using Envelope = DSP::EnvelopeClipExp<EnvelopeType, 1u>;
    using EnvelopeCache = Core::TinyVector<float>;

    /** @brief Store the cache of a note */
    struct alignas_quarter_cacheline NoteCache
    {
        NoteEvent::EventType type;
        NoteModifiers noteModifiers {};
    };

    // static_assert_fit_quarter_cacheline(NoteCache);

    using NoteCacheList = Core::SmallVector<NoteCache, 2u, std::uint8_t>;
    using NoteArray = std::array<NoteCacheList, KeyCount>;

    /** @brief Describe the internal cache */
    struct alignas_double_cacheline Cache
    {
        ActiveKeyList actives {};
        NoteArray modifiers;
        IndexArray readIndexes;
    };

    static_assert_alignof_double_cacheline(Cache);


    [[nodiscard]] std::uint32_t getActiveNoteSize(void) const noexcept { return _cache.actives.size(); }

    [[nodiscard]] ActiveKeyList &getActiveNote(void) noexcept { return _cache.actives; }

    [[nodiscard]] const ActiveKeyList &getActiveNote(void) const noexcept { return _cache.actives; }

    template<typename ProcessFunctor, typename TestFunctor, typename ResetFunctor>
    void processNotes(BufferView outputFrame, ProcessFunctor &&processFunctor, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor) noexcept
        { return processNotesImpl<false, ProcessFunctor, TestFunctor, ResetFunctor>(outputFrame, std::forward<ProcessFunctor>(processFunctor), std::forward<TestFunctor>(testFunctor), std::forward<ResetFunctor>(resetFunctor)); }

    template<typename ProcessFunctor, typename TestFunctor, typename ResetFunctor>
    void processNotesEnvelope(BufferView outputFrame, ProcessFunctor &&processFunctor, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor) noexcept
        { return processNotesImpl<true, ProcessFunctor, TestFunctor, ResetFunctor>(outputFrame, std::forward<ProcessFunctor>(processFunctor), std::forward<TestFunctor>(testFunctor), std::forward<ResetFunctor>(resetFunctor)); }


    /** @brief Process a list of notes and update the internal cache */
    void feedNotes(const NoteEvents &notes) noexcept;

    /** @brief Process a list of notes and update the internal cache - reset a key if its event is ON */
    template<typename ResetFunctor>
    void feedNotesRetrigger(const NoteEvents &notes, ResetFunctor &&resetFunctor) noexcept;

    /** @brief Reset all */
    void reset(void)
    {
        resetNoteCache();
        clearEnvelopeCache();
        resetAllModifiers();
        resetReadIndexes();
        _envelope.resetKeys();
    }

    /** @brief Reset the internal cache. All notes are turned off */
    void resetNoteCache(void) noexcept { _cache.actives.clear(); }

    /** @brief Clear the internal envelope cache. */
    void clearEnvelopeCache(void) noexcept { _envelopeGain.clear(); }

    /** @brief Reset the internal note modifiers */
    void resetNoteModifiers(void) noexcept;

    /** @brief Reset the internal note poly pressure modifiers */
    void resetPolyModifiers(void) noexcept;

    /** @brief Reset the internal modifiers. Both note and poly pressure modifiers are reset */
    void resetAllModifiers(void) noexcept;

    /** @brief Reset all read indexes */
    void resetReadIndexes(void) noexcept;

    /** @brief Increment the read index of given key */
    template<typename TestFunctor, typename ResetFunctor>
    [[nodiscard]] bool incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor) noexcept;


    /** @brief Get the read index of given key */
    [[nodiscard]] std::uint32_t readIndex(const Key key) const noexcept { return _cache.readIndexes[key]; }

    /** @brief Set the read index of given key */
    void setReadIndex(const Key key, const std::uint32_t index) noexcept { _cache.readIndexes[key] = index; }

    /** @brief Reset the read index of given key */
    void resetReadIndex(const Key key) noexcept { _cache.readIndexes[key] = 0u; }

    /** @brief Get the envelope gain of given key */
    [[nodiscard]] inline float getEnvelopeGain(
            const Key key, const std::uint32_t index) noexcept
    {
        return _envelope.template getGain<0u>(key, index);
    }

    /** @brief Generate envelope gains in the internal cache */
    void generateEnvelopeGains(const Key key, const std::uint32_t index, const std::size_t outputSize) noexcept;

    void setEnvelopeSpecs(const DSP::EnvelopeSpecs &specs) noexcept { _envelope.template setSpecs<0u>(specs); }

    [[nodiscard]] const Envelope &envelope(void) const noexcept { return _envelope; }
    [[nodiscard]] Envelope &envelope(void) noexcept { return _envelope; }

    [[nodiscard]] const EnvelopeCache &envelopeGain(void) const noexcept { return _envelopeGain; }
    [[nodiscard]] EnvelopeCache &envelopeGain(void) noexcept { return _envelopeGain; }

private:
    Cache _cache;
    Envelope _envelope;
    EnvelopeCache _envelopeGain;

    template<bool ProcessEnvelope, typename ProcessFunctor, typename TestFunctor, typename ResetFunctor>
    void processNotesImpl(BufferView outputFrame, ProcessFunctor &&processFunctor, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor) noexcept;

    template<bool ProcessEnvelope, typename ProcessFunctor, typename TestFunctor, typename ResetFunctor>
    bool processOneNoteImpl(
            ProcessFunctor &&processFunctor, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor,
            const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers,
            float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount
    ) noexcept;
};

// static_assert_alignof_double_cacheline(Audio::NoteManager);

#include "NoteManager.ipp"
