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

static const Audio::Device::Descriptor DefaultDeviceDescriptor {
    /*.name = */ "device-test",
    /*.blockSize = */ 2048u,
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

private:
    Scheduler _scheduler;
    Audio::Device _device { DefaultDeviceDescriptor, &Interpreter::AudioCallback };
    Audio::Device::Descriptor _deviceDescriptor { DefaultDeviceDescriptor };
    std::unordered_map<std::string_view, NodeHolder> _map {};

    static inline Core::SPSCQueue<std::uint8_t> _AudioCallbackBuffer { 16384 };
    static inline std::atomic<std::size_t> _AudioCallbackMissCount { 0u };

    static void AudioCallback(void *, std::uint8_t *stream, const int length);

    std::istringstream _is {};
    std::string _command {};
    std::string _word {};
    bool _running { false };

    void getNextCommand(void);
    void getNextWord(void);

    template<typename As>
    [[nodiscard]] As getNextWordAs(const char * const what);

    void parseCommand(void);

    void parseSettingsCommand(void);
    void parsePluginCommand(void);
    void parseNoteCommand(void);
    void parseControlCommand(void);

    [[nodiscard]] Core::HashedName getCurrentHashedWord(void) const noexcept { return Core::Hash(_word); }


    [[nodiscard]] const NodeHolder &getNode(const std::string_view &name) const;
    [[nodiscard]] NodeHolder &getNode(const std::string_view &name)
        { return const_cast<NodeHolder &>(const_cast<const Interpreter &>(*this).getNode(name)); }

    void removeNode(NodeHolder &node);
    void removeNodeImpl(Audio::Node &node) noexcept;
};
#include "Interpreter.ipp"