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

inline void Audio::Node::prepareCache(const AudioSpecs &specs)
{
    {
        const auto newSize = GetFormatByteLength(specs.format) * specs.processBlockSize;
        _cache.resize(newSize, specs.sampleRate, specs.channelArrangement, specs.format);
    }

    for (auto &child : _children) {
        child->prepareCache(specs);
    }
    _plugin->updateAudioSpecs(specs);
}

inline void Audio::Node::onAudioGenerationStarted(const BeatRange &range) noexcept
{
    // We process plugins from bottom to top
    for (auto &child : _children) {
        child->onAudioGenerationStarted(range);
    }
    plugin()->onAudioGenerationStarted(range);
}
