/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Node
 */

inline void Audio::Node::setPlugin(PluginPtr &&plugin)
{
    // reset affected members
    _plugin = std::move(plugin);
    _flags = _plugin->getFlags();
}

inline bool Audio::Node::setMuted(const bool muted) noexcept
{
    if (muted == _muted)
        return false;
    _muted = muted;
    return true;
}

inline bool Audio::Node::setColor(const Color color) noexcept
{
    if (color == _color)
        return false;
    _color = color;
    return true;
}

inline bool Audio::Node::setName(Core::FlatString &&name) noexcept
{
    if (name == _name)
        return false;
    _name = std::move(name);
    return true;
}

inline void Audio::Node::onAudioGenerationStarted(const BeatRange &range) noexcept
{
    // We process plugins from bottom to top
    for (auto &child : _children) {
        child->onAudioGenerationStarted(range);
    }
    plugin()->onAudioGenerationStarted(range);
}
