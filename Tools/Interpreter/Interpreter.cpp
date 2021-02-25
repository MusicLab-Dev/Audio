/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */


#include <Audio/PluginTable.hpp>

#include "Interpreter.hpp"

using namespace Core::Literal;

constexpr auto GeneralHelpText =
        "Usage: Command [SubCommand] [Parameters, ...]\n"
        "General command list:\n"
        "- play\n\tSet the generator state to play\n"
        "- pause\n\tSet the generator state to pause\n"
        "- stop\n\tSet the generator state to pause, and reset beat index\n"
        "- settings {SubCommand}\n\tChange global parameters\n"
        "- plugin {SubCommand}\n\tAdd / remove / modify plugins\n"
        "- note {SubCommand}\n\tAdd / remove / modify plugins' notes\n"
        "- control {SubCommand}\n\tModify plugins' controls\n"
        "- exit\n\tClose the interpreter\n"
        "- help\n\tModify plugins' controls\n";

constexpr auto SettingsHelpText =
        "Usage: settings [SubCommand] [Parameters, ...]\n"
        "Settings command list:\n"
        "- list\n\tList every settings parameters\n"
        "- sampleRate {Interger} (default: 44100)\n\tChange the sample rate\n"
        "- channelArrangement {'M': mono | 'S': stereo} (default: Mono)\n\tChange the channel arrangement rate\n"
        "- format {'F32' | 'I8' | 'I16' | 'I32' } (default: 'F32')\n\tChange the channel format rate\n"
        "- blockSize {Integer} (default: '2048')\n\tChange the audio callback block size\n";

constexpr auto PluginHelpText =
        "Usage: plugin [SubCommand] [Parameters, ...]\n"
        "Plugin command list:\n"
        "- list_factories\n\tList every registered factory\n"
        "- list_plugins\n\tList every instantiated factories\n"
        "- add {String: FactoryName} {String: Name} [{String: Parent name}]\n\tInstantiate a plugin (from master if no parent is specified)\n"
        "- remove {String: Name}\n\tDelete a plugin (and its children !)\n"
        "- move {String: Name} {String: New parent name}\n\tMove a plugin to another parent (and its children !)\n";

constexpr auto NoteHelpText =
        "Usage: note [SubCommand] [Parameters, ...]\n"
        "Note command list:\n"
        "- list_partitions {String: Plugin's name}\n\tList all partitions of a plugin\n"
        "- list_partition {String: Plugin's name} {Integer: Partition index}\n\tList all notes of a plugin's partition\n"
        "- create [{String: Plugin's name}]\n\tCreate a new partition into plugin\n"
        "- add {String: Plugin's name} {Integer: Key} {Integer: Beat from} {Integer: Beat to} {Integer: Velocity}\n\tAdd a note into a plugin's partition\n"
        "- remove {String: Plugin's Name} {Integer: Note index}\n\tDelete a note in a plugin's partition\n";

constexpr auto ControlHelpText =
        "Usage: control [SubCommand] [Parameters, ...]\n"
        "Control command list:\n"
        "- list {String: Plugin's name}\n\tList all controls of a plugin\n"
        "- get {String: Plugin's name} {Integer: Control index}\n\tGet a plugin's control\n"
        "- set {String: Plugin's name} {Integer: Control index} {Float: Control value}\n\tSet a plugin's control\n";

void Interpreter::AudioCallback(void *, std::uint8_t *stream, const int length)
{
    auto total = Interpreter::_AudioCallbackBuffer.popRange(stream, stream + length);

    if (total != length)
        ++_AudioCallbackMissCount;
}

void Interpreter::run(void)
{
    std::size_t audioCallbackMissCount { 0u };

    _running = true;
    while (_running) {
        try {
            if (audioCallbackMissCount < _AudioCallbackMissCount.load()) {
                audioCallbackMissCount = _AudioCallbackMissCount;
                std::cout << "An audio callback has been missed or uncompleted (n. " << audioCallbackMissCount << ')' << std::endl;
            }
            getNextCommand();
            _scheduler.addEvent([this] {
                parseCommand();
            });
        } catch (const std::logic_error &e) {
            std::cout << "Logic error thrown:\n\t" << e.what() << std::endl;
        }
    }
}

void Interpreter::parseCommand(void)
{
    getNextWord();
    switch (getCurrentHashedWord()) {
    case "settings"_hash:
        return parseSettingsCommand();
    case "plugin"_hash:
        return parsePluginCommand();
    case "note"_hash:
        return parseNoteCommand();
    case "control"_hash:
        return parseControlCommand();
    case "exit"_hash:
        _running = false;
        break;
    case "help"_hash:
    case "?"_hash:
        std::cout << GeneralHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parseCommand: Unknown command '" + _word + '\'');
    }
}



void Interpreter::parseSettingsCommand(void)
{
    getNextWord();
    switch (getCurrentHashedWord()) {
    case "list"_hash:
        std::cout << "Settings:" << std::endl;
        std::cout << "- Device name: " << _device.name() << std::endl;
        std::cout << "- Block size: " << _device.blockSize() << std::endl;
        std::cout << "- Sample rate: " << _device.sampleRate() << std::endl;
        std::cout << "- Format: " << static_cast<int>(_device.format()) << std::endl;
        std::cout << "- Midi channels: " << static_cast<int>(_device.midiChannels()) << std::endl;
        std::cout << "- Channel arrangement: " << (_device.channelArrangement() == Audio::ChannelArrangement::Mono ? "Mono" : "Stereo") << std::endl;
        break;
    case "sampleRate"_hash:
        _device.setSampleRate(getNextWordAs<int>("Interpreter::parseSettingsCommand: Invalid sample rate input value"));
        break;
    case "channelArrangement"_hash:
        getNextWord();
        switch (_word[0]) {
        case 'M':
            _device.setChannelArrangement(Audio::ChannelArrangement::Mono);
            break;
        case 'S':
            _device.setChannelArrangement(Audio::ChannelArrangement::Stereo);
            break;
        default:
            throw std::logic_error("Interpreter::parseSettingsCommand: Invalid 'channelArrangement' settings value '" + _word + '\'');
        }
        break;
    case "format"_hash:
    {
        getNextWord();
        switch (getCurrentHashedWord()) {
        case "F32"_hash:
            _device.setFormat(Audio::Format::Floating32);
            break;
        case "I32"_hash:
            _device.setFormat(Audio::Format::Fixed32);
            break;
        case "I16"_hash:
            _device.setFormat(Audio::Format::Fixed16);
            break;
        case "I8"_hash:
            _device.setFormat(Audio::Format::Fixed8);
            break;
        default:
            throw std::logic_error("Interpreter::parseSettingsCommand: Invalid 'format' settings value '" + _word + '\'');
        }
        break;
    }
    case "blockSize"_hash:
    {
        _device.setBlockSize(getNextWordAs<unsigned int>("Interpreter::parseSettingsCommand: Invalid blockSize input value"));
        break;
    }
    case "help"_hash:
    case "?"_hash:
        std::cout << SettingsHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parseSettingsCommand: Unknown settings command '" + _word + '\'');
    }

    // TODO: 'reload' device with the new parameters !
}

void Interpreter::parsePluginCommand(void)
{
    getNextWord();
    switch (getCurrentHashedWord()) {
    case "list_factories"_hash:
    {
        if (Audio::PluginTable::Get().factories().empty()) {
            std::cout << "Zero factory loaded." << std::endl;
            break;
        }
        std::cout << "Actual factories:" << std::endl;
        for (const auto &factory : Audio::PluginTable::Get().factories()) {
            std::cout << " - " << factory->getName() << std::endl;
        }
        break;
    }
    case "list_plugins"_hash:
    {
        if (_map.empty()) {
            std::cout << "Zero plugin loaded." << std::endl;
            break;
        }
        std::cout << "Actual plugins:" << std::endl;
        for (const auto &it : _map) {
            std::cout << " - " << it.first << std::endl;
        }
        break;
    }
    case "add"_hash:
    {
        getNextWord();
        const std::string factory = _word;
        getNextWord();
        const std::string name = _word;
        getNextWord();
        const std::string &parentName = _word;
        auto node = std::make_unique<Audio::Node>(Audio::PluginTable::Get().instantiate(factory, _deviceDescriptor.sampleRate, _deviceDescriptor.channelArrangement));
        node->setName(Core::FlatString(name));
        auto &parent = getNode(parentName);
        _map.insert(std::make_pair(std::string_view(name), NodeHolder { node.get(), parent.ptr }));
        parent.ptr->children().push(std::move(node));
        break;
    }
    case "remove"_hash:
    {
        getNextWord();
        removeNode(getNode(_word));
        break;
    }
    case "help"_hash:
    case "?"_hash:
        std::cout << PluginHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parsePluginCommand: Unknown plugin command '" + _word + '\'');
    }
}

void Interpreter::removeNode(NodeHolder &node)
{
    removeNodeImpl(*node.ptr);
    auto it = node.parent->children().find([&node](auto &it) { return it.get() == node.ptr; });
    if (it == node.parent->children().end())
        throw std::runtime_error("Interpreter::removeNode: Critical remove error");
    node.parent->children().erase(it);
}

void Interpreter::removeNodeImpl(Audio::Node &node) noexcept
{
    for (auto &child : node.children()) {
        removeNodeImpl(*child);
    }
    _map.erase(node.name().toStdView());
}

void Interpreter::parseNoteCommand(void)
{
    getNextWord();
    switch (getCurrentHashedWord()) {
    case "list"_hash:
        break;
    case "add"_hash:
        break;
    case "remove"_hash:
        break;
    case "move"_hash:
        break;
    case "help"_hash:
    case "?"_hash:
        std::cout << NoteHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parseNoteCommand: Unknown note command '" + _word + '\'');
    }
}

void Interpreter::parseControlCommand(void)
{
    getNextWord();
    switch (getCurrentHashedWord()) {
    case "list"_hash:
        break;
    case "get"_hash:
        break;
    case "set"_hash:
        break;
    case "help"_hash:
    case "?"_hash:
        std::cout << ControlHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parseControlCommand: Unknown control command '" + _word + '\'');
    }
}
