/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */


#include <thread>

#include <Audio/PluginTable.hpp>

#include <Audio/Plugins/Sampler.hpp>
#include <Audio/Plugins/Mixer.hpp>

#include "Interpreter.hpp"

using namespace Core::Literal;

constexpr auto GeneralHelpText =
        "Usage: Command [SubCommand] [Parameters, ...]\n"
        "General command list:\n"
        "- load {String: config file path}\n\tLoad a config file containing a set of commands\n"
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
        "- load_sample {String: PluginName} {String: SamplePath}\n\tLoad a sample for a sampler plugin\n"
        "- remove {String: Name}\n\tDelete a plugin (and its children !)\n"
        "- move {String: Name} {String: New parent name}\n\tMove a plugin to another parent (and its children !)\n";

constexpr auto NoteHelpText =
        "Usage: note [SubCommand] [Parameters, ...]\n"
        "Note command list:\n"
        "- list_partitions {String: Plugin's name}\n\tList all partitions of a plugin\n"
        "- list_partition {String: Plugin's name} {Integer: Partition index}\n\tList all notes of a plugin's partition\n"
        "- create [{String: Plugin's name}]\n\tCreate a new partition into plugin (into master if no plugin is specified)\n"
        "- add {String: Plugin's name} {PowerOf2: BeatPrecision} {Integer: Key} {Integer: Beat from} {Integer: Beat to} [{Integer: Velocity}]\n\tAdd a note into a plugin's partition\n"
        "- remove {String: Plugin's Name} {Integer: Note index}\n\tDelete a note in a plugin's partition\n";

constexpr auto ControlHelpText =
        "Usage: control [SubCommand] [Parameters, ...]\n"
        "Control command list:\n"
        "- list {String: Plugin's name}\n\tList all controls of a plugin\n"
        "- get {String: Plugin's name} {Integer: Control index}\n\tGet a plugin's control\n"
        "- set {String: Plugin's name} {Integer: Control index} {Float: Control value}\n\tSet a plugin's control\n";


Interpreter::Interpreter(void)
    : _device(DefaultPhysicalDescriptor, &Interpreter::AudioCallback)
{
    registerInternalFactories();
    prepareCache();
}

Interpreter::~Interpreter(void)
{
    std::cout << "Waiting for scheduler...." << std::endl;
    _scheduler.setState(Scheduler::State::Pause);
    _scheduler.wait();
    std::cout << "Success !" << std::endl;
}

void Interpreter::prepareCache(void)
{

    // _scheduler.setProcessBeatSize(beatsNorm);
    _scheduler.setProcessParamByBlockSize(1024u, getAudioSpecs().sampleRate);

    // _scheduler.setIsLooping(false);
    // _scheduler.setLoopBeatRange(MakeBeatRange(0u, 4u, NoteType::QuarterNote));

    const auto specs = getAudioSpecs();
    _scheduler.prepareCache(specs);

    std::cout << "scheduler::processBeatSize: " << _scheduler.processBeatSize() << std::endl;
    std::cout << "scheduler::LoopingRange: " << _scheduler.loopBeatRange() << std::endl;
    std::cout << "beatRange: " << _scheduler.getCurrentBeatRange() << std::endl;
    // exit(0);
}

void Interpreter::registerInternalFactories(void) noexcept
{
    static constexpr char MixerFactoryName[] = "__internal__:/Mixer";
    static constexpr char MixerNodeName[] = "master";

    insertNode(nullptr, Audio::PluginTable::Get().instantiate(MixerFactoryName), MixerNodeName);
}

void Interpreter::AudioCallback(std::uint8_t *stream, const std::size_t length)
{
    auto total = Scheduler::ConsumeAudioData(stream, length);

    if (total != length) {
        ++_AudioCallbackMissCount;
        // std::cout << "AudioCallback MISS " << total << " / " << length << std::endl;
    }
    // auto count = 0;
    // for (auto i = 0; i < length; ++i) {
    //     if (stream[i])
    //         ++count;
    // }
    // if (count)
    //     std::cout << "< Audio Callback non-null > " << count << std::endl;
}

void Interpreter::run(void)
{
    std::size_t audioCallbackMissCount { 0u };

    _running = true;
    _is.clear();
    _is.str("load Tools/dnb.txt");
    parseCommand();
    _scheduler.invalidateCurrentGraph();

    while (_running) {
        try {
            if (audioCallbackMissCount < _AudioCallbackMissCount.load()) {
                audioCallbackMissCount = _AudioCallbackMissCount;
                std::cout << "An audio callback has been missed or uncompleted (n. " << audioCallbackMissCount << ')' << std::endl;
            }

            getNextCommand();
            parseCommand();
            // Sleep for the requested duration if specified by a command
            if (_sleepFor) {
                std::cout << "sleeping time" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(_sleepFor));
                _sleepFor = 0u;
                std::cout << "wake up!" << std::endl;
            }
        } catch (const std::logic_error &e) {
            std::cout << "Logic error thrown:\n\t" << e.what() << std::endl;
        }
    }
}

void Interpreter::parseCommand(void)
{
    getNextWord();
    if (_word[0] == '#')
        return;
    switch (getCurrentHashedWord()) {
    case "load"_hash:
        return parseLoadCommand();
    case "settings"_hash:
        return parseSettingsCommand();
    case "plugin"_hash:
        return parsePluginCommand();
    case "note"_hash:
        return parseNoteCommand();
    case "control"_hash:
        return parseControlCommand();
    case "exit"_hash:
    {
        _scheduler.setState(Audio::AScheduler::State::Pause);
        _scheduler.wait();
        _running = false;
        break;
    }
    case "play"_hash:
        return parseRunningCommand(AudioState::Play);
        break;
    case "pause"_hash:
        return parseRunningCommand(AudioState::Pause);
    case "stop"_hash:
        return parseRunningCommand(AudioState::Stop);
    case "sleep"_hash:
        _sleepFor = getNextWordAs<std::size_t>("Interpreter::parseCommand::sleep: Invalid sleep duration");
        break;
    case "playFor"_hash:
    {
        parseRunningCommand(AudioState::Play);
        _sleepFor = getNextWordAs<std::size_t>("Interpreter::parseCommand::sleep: Invalid sleep duration");
        std::this_thread::sleep_for(std::chrono::milliseconds(_sleepFor));
        _sleepFor = 0u;
        parseRunningCommand(AudioState::Pause);
        break;
    }
    case "help"_hash:
    case "?"_hash:
        std::cout << GeneralHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parseCommand: Unknown command '" + _word + '\'');
    }
}
void Interpreter::parseLoadCommand(void)
{
    getNextWord();
    std::ifstream fileStream;
    fileStream.open(_word);
    if (!fileStream.is_open())
        throw std::logic_error("Interpreter::parseLoadCommand: Invalid file path '" + _word + '\'');
    std::string readLine;
    while (std::getline(fileStream, readLine)) {
        if (readLine.substr(0, 4) == "load") {
            std::cout << "Interpreter::parseLoadCommand: Couldn't load another commands file" << std::endl;
            break;
        }
        _is.clear();
        _is.str(readLine);
        parseCommand();
            // Sleep for the requested duration if specified by a command
        if (_sleepFor) {
            std::cout << "sleeping time" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(_sleepFor));
            _sleepFor = 0u;
            std::cout << "wake up!" << std::endl;
        }
    }
}

void Interpreter::parseRunningCommand(const AudioState state)
{
    if (state == AudioState::Play) {
        if (_audioState == AudioState::Play) {
            std::cout << "Interpreter::parseRunningCommand: Scheduler is already running" << std::endl;
            return;
        }
        _audioState = state;
        _device.start(); // audioState c pour le device et aussi le scheduler
        std::cout << " <Play>" << std::endl;
        _scheduler.setState(Audio::AScheduler::State::Play);
        return;
    } else if (state == AudioState::Pause) {
        if (_audioState != AudioState::Play) {
            std::cout << "Interpreter::parseRunningCommand: Scheduler is already paused" << std::endl;
            return;
        }
        _audioState = state;
        _device.stop();
        std::cout << " <Pause>" << std::endl;
        _scheduler.setState(Audio::AScheduler::State::Pause);
        _scheduler.wait();
        return;
    }
    _scheduler.currentBeatRange<Audio::PlaybackMode::Production>() = { 0u, _scheduler.processBeatSize() };
    _scheduler.clearAudioQueue();
    _scheduler.clearOverflowCache();
}


void Interpreter::parseSettingsCommand(void)
{
    bool changed = false;

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
        changed = true;
        break;
    case "channelArrangement"_hash:
        getNextWord();
        switch (_word[0]) {
        case 'M':
            _device.setChannelArrangement(Audio::ChannelArrangement::Mono);
            changed = true;
            break;
        case 'S':
            _device.setChannelArrangement(Audio::ChannelArrangement::Stereo);
            changed = true;
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
            changed = true;
            break;
        case "I32"_hash:
            _device.setFormat(Audio::Format::Fixed32);
            changed = true;
            break;
        case "I16"_hash:
            _device.setFormat(Audio::Format::Fixed16);
            changed = true;
            break;
        case "I8"_hash:
            _device.setFormat(Audio::Format::Fixed8);
            changed = true;
            break;
        default:
            throw std::logic_error("Interpreter::parseSettingsCommand: Invalid 'format' settings value '" + _word + '\'');
        }
        break;
    }
    case "blockSize"_hash:
        _device.setBlockSize(getNextWordAs<unsigned int>("Interpreter::parseSettingsCommand: Invalid blockSize input value"));
        changed = true;
        break;
    case "processBlockSize"_hash:
        _scheduler.setProcessParamByBlockSize(getNextWordAs<unsigned int>("Interpreter::parseSettingsCommand: Invalid blockSize input value"), getAudioSpecs().sampleRate);
        changed = true;
    case "loopRange"_hash:
    {
        const auto beatPrecision = getNextWordAs<unsigned int>("Interpreter::parseSettingsCommand::loopRange: Invalid beatPrecision");
        const auto from = getNextWordAs<unsigned int>("Interpreter::parseSettingsCommand::loopRange: Invalid range from");
        const auto to = getNextWordAs<unsigned int>("Interpreter::parseSettingsCommand::loopRange: Invalid range to");
        _scheduler.setLoopBeatRange(MakeBeatRange(from, to, static_cast<NoteType>(beatPrecision)));
        changed = true;
        break;
    }
    case "loop"_hash:
        _scheduler.setIsLooping(getNextWordAs<bool>("Interpreter::parseSettingsCommand::loop: Invalid boolean value"));
        break;
    case "help"_hash:
    case "?"_hash:
        std::cout << SettingsHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parseSettingsCommand: Unknown settings command '" + _word + '\'');
    }

    if (!changed)
        return;
    else if (!_running) {
        _device.reloadDevice();
        prepareCache();
    } else {
        _device.stop();
        _device.reloadDevice();
        prepareCache();
        _device.start();
    }
}

void Interpreter::parsePluginCommand(void)
{
    bool changed = false;

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
// plugin add Sampler MySampler

        getNextWord();
        const std::string factory = "__internal__:/" + _word;
        getNextWord();
        const std::string name = _word;
        bool hasParent = getNextWordNoThrow();
        const std::string &parentName = (hasParent ? _word : "master");

        Audio::AudioSpecs specs {
            /* .sampleRate = */         _deviceDescriptor.sampleRate,
            /* .channelArrangement = */ _deviceDescriptor.channelArrangement,
            /* .format = */             _deviceDescriptor.format,
            /* .processBlockSize = */   _scheduler.processBlockSize()
        };

        insertNode(getNode(parentName).ptr, Audio::PluginTable::Get().instantiate(factory), name);
        changed = true;
        break;
    }
    case "load_sample"_hash:
    {
        getNextWord();
        const auto pluginName = _word;
        getNextWord();
        const auto samplePath = _word;
        auto &node = getNode(pluginName);
        reinterpret_cast<Audio::Sampler *>(node.ptr->plugin())->loadSample<float>(samplePath);
        changed = true;
        break;
    }
    case "remove"_hash:
    {
        getNextWord();
        removeNode(getNode(_word));
        changed = true;
        break;
    }
    case "help"_hash:
    case "?"_hash:
        std::cout << PluginHelpText << std::endl;
        break;
    default:
        throw std::logic_error("Interpreter::parsePluginCommand: Unknown plugin command '" + _word + '\'');
    }
    if (changed)
        _scheduler.invalidateCurrentGraph();
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
    _map.erase(node.name().toStdString());
}

void Interpreter::parseNoteCommand(void)
{
    getNextWord();
    switch (getCurrentHashedWord()) {
    case "list_partitions"_hash:
    {
        getNextWord();
        auto &node = getNode(_word);
        if (node.ptr->partitions().empty()) {
            std::cout << "Zero partitions loaded for node '" << _word << "'" << std::endl;
            return;
        }
        std::cout << "Partitions for node '" << _word << "':" << std::endl;
        for (auto partition : node.ptr->partitions()) {
            std::cout << "- notes:" << std::endl;
            for (auto note : partition.notes()) {
                std::cout << "    - " << note << std::endl;
            }

            std::cout << "- instances:" << std::endl;
            for (auto instance : partition.instances()) {
                std::cout << "    - " << instance.from << ":" << instance.to << std::endl;
            }
            std::cout << std::endl;
        }
        break;
    }
    case "list_partition"_hash:
        break;
    case "create"_hash:
    {
        bool isPlugin = getNextWordNoThrow();
        const auto beatPrecision = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::create: Invalid beatPrecision parameter");
        // Check if beatPrecision is a power of two
        if (!beatPrecision || (beatPrecision & (beatPrecision - 1)))
            throw std::logic_error("Interpreter::parseNoteCommand::create: first argument must be a power of 2.");
        const auto from = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::create: Invalid beat from parameter");
        const auto to = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::create: Invalid beat to parameter");
        const std::string &nodeName = (isPlugin ? _word : "master");
        auto &node = getNode(nodeName);
        Audio::Partition partition;
        auto inst = partition.instances().push(MakeBeatRange(from, to, static_cast<NoteType>(beatPrecision)));
        std::cout << "range: " << inst << std::endl;
        node.ptr->partitions().push(std::move(partition));
        break;
    }
    case "add"_hash:
    {
        getNextWord();
        auto &node = getNode(_word);
        const auto partitionIndex = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::add: Invalid partitionIndex parameter");
        const auto key = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::add: Invalid partitionIndex parameter");
        const auto beatPrecision = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::add: Invalid beatPrecision parameter");
        // Check if beatPrecision is a power of two
        if (!beatPrecision || (beatPrecision & (beatPrecision - 1)))
            throw std::logic_error("Interpreter::parseNoteCommand::add: first argument must be a power of 2.");
        const auto from = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::add: Invalid beat from parameter");
        const auto to = getNextWordAs<unsigned int>("Interpreter::parseNoteCommand::add: Invalid beat to parameter");

        Audio::Note note;
        note.key = key;
        note.range = MakeBeatRange(from, to, static_cast<NoteType>(beatPrecision));
        std::cout << "note range: " << note.range << std::endl;
        // note.range = Audio::BeatRange { 0, 20'000 };
        node.ptr->partitions()[partitionIndex].notes().push(note);
        break;
    }
    case "remove"_hash:
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

Audio::NodePtr &Interpreter::insertNode(Audio::Node *parent, Audio::PluginPtr &&plugin, const std::string_view &name)  noexcept
{
    const auto specs = getAudioSpecs();

    Audio::NodePtr *node;

    if (parent)
        node = &(parent->children().push(std::make_unique<Audio::Node>(parent, std::move(plugin))));
    else
        node = &(_scheduler.project()->master() = std::make_unique<Audio::Node>(nullptr, std::move(plugin)));
    (*node)->setName(Core::FlatString(name));
    (*node)->prepareCache(specs);
    _map.insert(std::make_pair(std::string_view(name), NodeHolder { node->get(), parent }));
    return *node;
}
