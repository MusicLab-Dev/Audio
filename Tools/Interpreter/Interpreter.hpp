/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#pragma once

#include <sstream>

#include <Core/Hash.hpp>
#include <Core/SPSCQueue.hpp>
#include <Audio/Device.hpp>

#include "Scheduler.hpp"
#include "Base.hpp"

static const Audio::Device::LogicalDescriptor DefaultPhysicalDescriptor {
    /*.name = */ "",
    /*.blockSize = */ 1024u,
    /*.sampleRate = */ 44100,
    /*.isInput = */ false,
    /*.format = */ Audio::Format::Floating32,
    /*.midiChannels = */ 2u,
    /*.channelArrangement = */ Audio::ChannelArrangement::Mono
};

class Interpreter
{
public:
    struct NodeHolder
    {
        Audio::Node *ptr { nullptr };
        Audio::Node *parent { nullptr };
    };

    void run(void);

    enum class AudioState {
        Play, Pause, Stop
    };

    Interpreter(void);
    ~Interpreter(void);

private:
    Scheduler _scheduler;
    Audio::Device _device;
    Audio::Device::LogicalDescriptor _deviceDescriptor { DefaultPhysicalDescriptor };
    std::unordered_map<std::string, NodeHolder> _map {};

    static inline std::atomic<std::size_t> _AudioCallbackMissCount { 0u };

    static void AudioCallback(std::uint8_t *stream, const std::size_t length);

    std::istringstream _is {};
    std::string _command {};
    std::string _word {};
    std::size_t _sleepFor { 0u };
    AudioState _audioState { AudioState::Stop };
    bool _running { false };

    void prepareCache(void);

    [[nodiscard]] Audio::AudioSpecs getAudioSpecs(void) const noexcept;

    void getNextCommand(void);
    void getNextWord(void);
    bool getNextWordNoThrow(void) noexcept;

    template<typename As>
    [[nodiscard]] As getNextWordAs(const char * const what);


    void parseCommand(void);

    void parseLoadCommand(void);
    void parseRunningCommand(const AudioState state);
    void parseSettingsCommand(void);
    void parsePluginCommand(void);
    void parseNoteCommand(void);
    void parseControlCommand(void);

    [[nodiscard]] Core::HashedName getCurrentHashedWord(void) const noexcept { return Core::Hash(_word); }


    [[nodiscard]] const NodeHolder &getNode(const std::string &name) const;
    [[nodiscard]] NodeHolder &getNode(const std::string &name)
        { return const_cast<NodeHolder &>(const_cast<const Interpreter &>(*this).getNode(name)); }

    void removeNode(NodeHolder &node);
    void removeNodeImpl(Audio::Node &node) noexcept;

    void registerInternalFactories(void) noexcept;

    Audio::NodePtr &insertNode(Audio::Node *parent, Audio::PluginPtr &&plugin, const std::string_view &name) noexcept;
};
#include "Interpreter.ipp"