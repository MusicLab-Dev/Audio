/*
 * @ Author: Pierre Veysseyre
 * @ Description: Node
 */

#pragma once

#include <memory>

#include "PluginTable.hpp"
#include "Control.hpp"
#include "Partition.hpp"
#include "Connection.hpp"
#include "Buffer.hpp"

namespace Audio
{
    class Node;

    /** @brief Handler to a node */
    using NodePtr = std::unique_ptr<Node>;

    /** @brief A list of nodes */
    using Nodes = Core::FlatVector<NodePtr>;

    /** @brief A list of connections */
    using Connections = Core::FlatVector<Connection>;

    /** @brief A list of partitions */
    using Partitions = Core::FlatVector<Partition>;
};

/** @brief A node contains a plugin, a partition table and an automation table */
class alignas_cacheline Audio::Node
{
public:
    /** @brief Default constructor */
    Node(void) noexcept = default;

    /** @brief Construct a node using an explicit plugin */
    Node(PluginPtr &&plugin) noexcept { setPlugin(std::move(plugin)); }

    /** @brief Move constructor */
    Node(Node &&other) noexcept = default;

    /** @brief Move assignment */
    Node &operator=(Node &&other) noexcept = default;

    /** @brief Get the internal plugin */
    [[nodiscard]] IPlugin *getPlugin(void) { return _plugin.get(); }

    /** @brief Set the internal plugin */
    void setPlugin(PluginPtr &&plugin);


    /** @brief Check if the node is muted (not active) or not */
    [[nodiscard]] bool muted(void) const noexcept { return _muted; }

    /** @brief Set the muted state of the node */
    void setMuted(const bool muted) noexcept { _muted = muted; }


    /** @brief Get the plugin's flags associated to this node */
    [[nodiscard]] const IPlugin::Flags flags(void) const noexcept { return _flags; }


    /** @brief Get / Set the color associated to this node */
    [[nodiscard]] Color color(void) const noexcept { return _color; }
    void setColor(const Color color) noexcept { _color = color; }


    /** @brief Get / Set the name of the node */
    [[nodiscard]] const Core::FlatString &name(void) const noexcept { return _name; }
    void setName(Core::FlatString &&name) noexcept { _name = name; }


    /** @brief Get a reference to the node plugin */
    /** @todo replace by Plugin ref */
    [[nodiscard]] PluginPtr &plugin(void) noexcept { return _plugin; }
    [[nodiscard]] const PluginPtr &plugin(void) const noexcept { return _plugin; }


    /** @brief Get a reference to the node partitions */
    [[nodiscard]] Partitions &partitions(void) noexcept { return _partitions; }
    [[nodiscard]] const Partitions &partitions(void) const noexcept { return _partitions; }


    /** @brief Get a reference to the node controls */
    [[nodiscard]] Controls &controls(void) noexcept { return _controls; }
    [[nodiscard]] const Controls &controls(void) const noexcept { return _controls; }


    /** @brief Get a reference to the node childrens */
    [[nodiscard]] Nodes &children(void) noexcept { return _children; }
    [[nodiscard]] const Nodes &children(void) const noexcept { return _children; }


    /** @brief Get a reference to the node connections */
    [[nodiscard]] Connections &connections(void) noexcept { return _connections; }
    [[nodiscard]] const Connections &connections(void) const noexcept { return _connections; }


    /** @brief Get a reference to the node cache */
    [[nodiscard]] Buffer &cache(void) noexcept { return _cache; }
    [[nodiscard]] const Buffer &cache(void) const noexcept { return _cache; }

    /** @brief Prepare the internal cache for a given audio output specifications
     *  Note that this function will recusrively call itself for every sub-children */
    void prepareCache(const AudioSpecs &specs);

    /** @brief Signal called when the generation of the audio block start */
    void onAudioGenerationStarted(const BeatRange &range) noexcept;

private:
    PluginPtr           _plugin { nullptr }; // 8
    Nodes               _children {}; // 8
    Connections         _connections {}; // 8
    Partitions          _partitions {}; // 8
    Buffer              _cache; // 8
    Controls            _controls {}; // 8
    bool                _muted { false }; // 1
    bool                _dirty { false }; // 1
    IPlugin::Flags      _flags {}; // 2
    Color               _color {}; // 4
    Core::FlatString    _name {}; // 8
};

#include "Node.ipp"
