/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler
 */

#pragma once

#include "Audio/AScheduler.hpp"

class Scheduler : public Audio::AScheduler
{
public:
    Scheduler(void) : Audio::AScheduler(std::make_unique<Audio::Project>(Core::FlatString("Interpreter Project"))) {}

    ~Scheduler(void) override = default;

    void onAudioBlockGenerated(void) override {
        // std::cout << "<onAudioBlockGenerated>" << std::endl;
        // std::cout << _currentBeatRange << std::endl;
    }
};