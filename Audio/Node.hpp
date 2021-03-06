/*
 * @ Author: Pierre Veysseyre
 * @ Description: Node
 */

#pragma once

#include <memory>

#include "PluginTable.hpp"
#include "Automations.hpp"
#include "Partitions.hpp"
#include "Connection.hpp"
#include "Buffer.hpp"

namespace Audio
{
    class Node;

    /** @brief Handler to a node */
    using NodePtr = std::unique_ptr<Node>;

    /** @brief A list of nodes */
    using Nodes = Core::FlatVector<NodePtr>;
}

/** @brief A node contains a plugin, a partition table and an automation table */
class  Audio::Node
{
public:
    /** @brief Number of partitions reserved at node creation */
    static constexpr std::uint32_t PartitionReservedCount = [] {
        std::uint32_t count = ((sizeof(Partitions) + sizeof(Partitions::Header)) % Core::CacheLineSize) / sizeof(Partition);
        return count ? count : Core::CacheLineSize / sizeof(Partition);
    }();

    /** @brief Default constructor */
    Node(Node * const parent) noexcept
        : _parent(parent) { _partitions.reserve(PartitionReservedCount); }

    /** @brief Construct a node using an explicit plugin */
    Node(Node * const parent, PluginPtr &&plugin) noexcept : Node(parent) { setPlugin(std::move(plugin)); }

    /** @brief Move constructor */
    Node(Node &&other) noexcept = default;

    /** @brief Move assignment */
    Node &operator=(Node &&other) noexcept = default;


    /** @brief Get the internal plugin */
    [[nodiscard]] IPlugin *plugin(void) { return _plugin.get(); }

    /** @brief Set the internal plugin */
    void setPlugin(PluginPtr &&plugin);


    /** @brief Get parent node */
    [[nodiscard]] Node *parent(void) const noexcept { return _parent; }

    /** @brief Set parent node */
    void setParent(Node * const parent) noexcept { _parent = parent; }


    /** @brief Check if the node is muted (not active) or not */
    [[nodiscard]] bool muted(void) const noexcept { return _muted; }

    /** @brief Set the muted state of the node */
    void setMuted(const bool muted) noexcept { _muted = muted; }


    /** @brief Get the plugin's flags associated to this node */
    [[nodiscard]] IPlugin::Flags flags(void) const noexcept { return _flags; }


    /** @brief Get / Set the color associated to this node */
    [[nodiscard]] Color color(void) const noexcept { return _color; }
    void setColor(const Color color) noexcept { _color = color; }


    /** @brief Get / Set the name of the node */
    [[nodiscard]] const Core::FlatString &name(void) const noexcept { return _name; }
    void setName(Core::FlatString &&name) noexcept { _name = name; }


    /** @brief Get a reference to the node partitions */
    [[nodiscard]] Partitions &partitions(void) noexcept { return _partitions; }
    [[nodiscard]] const Partitions &partitions(void) const noexcept { return _partitions; }


    /** @brief Get a reference to the node automations */
    [[nodiscard]] Automations &automations(void) noexcept { return _automations; }
    [[nodiscard]] const Automations &automations(void) const noexcept { return _automations; }


    /** @brief Get a reference to the node childrens */
    [[nodiscard]] Nodes &children(void) noexcept { return _children; }
    [[nodiscard]] const Nodes &children(void) const noexcept { return _children; }


    /** @brief Get a reference to the node cache */
    [[nodiscard]] Buffer &cache(void) noexcept { return _cache; }
    [[nodiscard]] const Buffer &cache(void) const noexcept { return _cache; }

    /** @brief Prepare the internal cache for a given audio output specifications
     *  Note that this function will recusrively call itself for every sub-children */
    void prepareCache(const AudioSpecs &specs);


    /** @brief Signal called when the generation of the audio block start */
    void onAudioGenerationStarted(const BeatRange &range) noexcept;

private:
    Node               *_parent { nullptr }; // 8
    PluginPtr           _plugin { nullptr }; // 8
    Nodes               _children {}; // 8
    Partitions          _partitions {}; // 8
    Buffer              _cache; // 8
    Automations         _automations {}; // 8
    bool                _muted { false }; // 1
    bool                _dirty { false }; // 1
    IPlugin::Flags      _flags {}; // 2
    Color               _color {}; // 4
    Core::FlatString    _name {}; // 8
    // Gain _gain;
};

#include "Node.ipp"
