/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler
 */

#pragma once

#include "Audio/AScheduler.hpp"

class Scheduler : public Audio::AScheduler
{
public:
    static constexpr std::size_t DefaultProcessBlockSize = 1024;

    Scheduler(void) : Audio::AScheduler(std::make_unique<Audio::Project>(Core::FlatString("Interpreter Project"))) {}

    ~Scheduler(void) override = default;

    void onAudioBlockGenerated(void) override;
    void onAudioQueueBusy(void) override;

    /** @brief Get / Set the process block size */
    [[nodiscard]] std::size_t processBlockSize(void) const noexcept { return _processBlockSize; }
    bool setProcessBlockSize(const std::size_t processBlockSize) noexcept;

    /** @brief Get / Set the loop beat range */
    [[nodiscard]] Audio::BeatRange loopBeatRange(void) const noexcept { return _loopBeatRange; }
    bool setLoopBeatRange(const Audio::BeatRange loopBeatRange) noexcept;

    /** @brief Get / Set the loop status */
    [[nodiscard]] bool isLooping(void) const noexcept { return _isLooping; }
    bool setIsLooping(const bool loopBeatRange) noexcept;



private:
    std::size_t _processBlockSize { DefaultProcessBlockSize };
    Audio::BeatRange _loopBeatRange {};
    bool _isLooping { true };
    Audio::Buffer _cache {};
};
