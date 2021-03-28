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

    /** @brief PlaybackMode describe project purpose
     * Production is used when the user want to create art !
     * Live is used when the user want to share its art :)
     */
    enum class PlaybackMode : std::uint32_t {
        Production, Live, Partition
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
    void setPlaybackMode(const PlaybackMode mode) noexcept { _playbackMode = mode; }

    /** @brief Get / Set the tempo */
    [[nodiscard]] Tempo tempo(void) const noexcept { return _bpm / 60.f; }
    void setTempo(const Tempo tempo) noexcept { _bpm = _bpm * 60.0f; }

    /** @brief Get / Set the BPM */
    [[nodiscard]] BPM bpm(void) const noexcept { return _bpm; }
    void setBPM(const BPM bpm) noexcept { _bpm = bpm; }

    /** @brief Get / Set the partition node */
    [[nodiscard]] Node *partitionNode(void) const noexcept { return _partitionNode; }
    void setPartitionNode(Node * const partitionNode) noexcept { _partitionNode = partitionNode; }

    /** @brief Get / Set the partition index */
    [[nodiscard]] std::uint32_t partitionIndex(void) const noexcept { return _partitionIndex; }
    void setPartitionIndex(const std::uint32_t partitionIndex) noexcept { _partitionIndex = partitionIndex; }

    /** @brief Signal called when the generation of the audio block start */
    void onAudioGenerationStarted(const BeatRange &range);

private:
    NodePtr             _master {};
    PlaybackMode        _playbackMode { PlaybackMode::Production };
    BPM                 _bpm { 240.0f / 4 };
    Core::FlatString    _name {};
    Node                *_partitionNode { nullptr };
    std::uint32_t       _partitionIndex { 0 };
};

static_assert_fit_cacheline(Audio::Project);

#include "Project.ipp"
