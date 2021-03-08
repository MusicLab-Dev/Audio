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

    /** @brief Get / Set the process block size */
    [[nodiscard]] std::size_t processBlockSize(void) const noexcept { return _processBlockSize; }
    [[nodiscard]] bool setProcessBlockSize(const std::size_t processBlockSize) noexcept;

private:
    std::size_t _processBlockSize { DefaultProcessBlockSize };
};



/**
 *
 * Ligne 69 Interpreter.cpp <-- (device block size + BPM + device samplerate == la <beatrange>)

 */