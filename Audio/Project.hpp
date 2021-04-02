/**
 * @ Author: Pierre Veysseyre
 * @ Description: Project.hpp
 */

#pragma once

#include <memory>

#include "FlatNode.hpp"

namespace Audio
{
    class Project;

    using ProjectPtr = std::shared_ptr<Project>;
};

class alignas_cacheline Audio::Project
{
public:
    static constexpr BPM MaxBpmSupported = 400;
    static constexpr BPM MinBpmSupported = 1;

    /** @brief Construct a new project with a given name */
    Project(Core::FlatString &&name) noexcept
        : _name(std::move(name)) {}

    /** @brief Destroy the project */
    ~Project(void) = default;

    /** @brief Get the master node */
    [[nodiscard]] NodePtr &master(void) noexcept { return _master; }
    [[nodiscard]] const NodePtr &master(void) const noexcept { return _master; }

    /** @brief Get the name node */
    [[nodiscard]] Core::FlatString &name(void) noexcept { return _name; }
    [[nodiscard]] const Core::FlatString &name(void) const noexcept { return _name; }

    /** @brief Get / Set the tempo */
    [[nodiscard]] Tempo tempo(void) const noexcept { return _bpm / 60.f; }
    void setTempo(const Tempo tempo) noexcept { _bpm = _bpm * 60.0f; }

    /** @brief Get / Set the BPM */
    [[nodiscard]] BPM bpm(void) const noexcept { return _bpm; }
    void setBPM(const BPM bpm) noexcept { _bpm = bpm; }

    /** @brief Signal called when the generation of the audio block start */
    void onAudioGenerationStarted(const BeatRange &range);

private:
    NodePtr             _master {};
    PlaybackMode        _playbackMode { PlaybackMode::Production };
    BPM                 _bpm { 150.f };
    Core::FlatString    _name {};
};

static_assert_fit_cacheline(Audio::Project);

#include "Project.ipp"
