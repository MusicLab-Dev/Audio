/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler
 */

#pragma once

#include "Audio/AScheduler.hpp"

class Scheduler : public Audio::AScheduler
{
public:
    static constexpr std::size_t DefaultProcessBlockSize = 1024u;

    Scheduler(void) : Audio::AScheduler(std::make_unique<Audio::Project>(Core::FlatString("Interpreter Project"))) {}

    ~Scheduler(void) override = default;

    bool onAudioBlockGenerated(void) override;
    bool onAudioQueueBusy(void) override;

private:
    Audio::Buffer _cache {};
};
