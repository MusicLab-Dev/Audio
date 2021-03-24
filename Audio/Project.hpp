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

class alignas_half_cacheline Audio::Project
{
public:
    static constexpr BPM MaxBpmSupported = 400;
    static constexpr BPM MinBpmSupported = 1;

    /** @brief PlaybackMode describe project purpose
     * Production is used when the user want to create art !
     * Live is used when the user want to share its art :)
     */
    enum class PlaybackMode : std::uint32_t {
        Production, Live
    };

    /** @brief Construct a new project with a given name */
    Project(Core::FlatString &&name, PlaybackMode mode = PlaybackMode::Production) noexcept
        : _playbackMode(mode), _name(std::move(name)) {}

    /** @brief Destroy the project */
    ~Project(void) = default;

    /** @brief Get the master node */
    [[nodiscard]] NodePtr &master(void) noexcept { return _master; }
    [[nodiscard]] const NodePtr &master(void) const noexcept { return _master; }

    /** @brief Get the name node */
    [[nodiscard]] Core::FlatString &name(void) noexcept { return _name; }
    [[nodiscard]] const Core::FlatString &name(void) const noexcept { return _name; }

    /** @brief Get / Set the playback mode */
    [[nodiscard]] PlaybackMode playbackMode(void) const noexcept { return _playbackMode; }
    bool setPlaybackMode(const PlaybackMode mode) noexcept;

    /** @brief Get / Set the tempo */
    [[nodiscard]] Tempo tempo(void) const noexcept { return _bpm / 60.f; }
    bool setTempo(const Tempo tempo) noexcept;

    /** @brief Get / Set the BPM mode */
    [[nodiscard]] BPM bpm(void) const noexcept { return _bpm; }
    bool setBPM(const BPM bpm) noexcept;

    /** @brief Signal called when the generation of the audio block start */
    void onAudioGenerationStarted(const BeatRange &range);

private:
    NodePtr             _master {};
    PlaybackMode        _playbackMode { PlaybackMode::Production };
    Core::FlatString    _name {};
    BPM                 _bpm { 240.0f / 4 };
};

static_assert_fit_half_cacheline(Audio::Project);

#include "Project.ipp"
