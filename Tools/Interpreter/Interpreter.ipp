/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

inline Audio::AudioSpecs Interpreter::getAudioSpecs(void) const noexcept
{
    return Audio::AudioSpecs {
        /* sampleRate: */ _device.sampleRate(),
        /* channelArrangement: */ _device.channelArrangement(),
        /* format: */ _device.format(),
        /* processBlockSize: */ _scheduler.processBlockSize()
    };
}

inline void Interpreter::getNextCommand(void)
{
    if (!std::getline(std::cin, _command))
        _running = false;
    _is.clear();
    _is.str(_command);
    if (_is.fail())
        throw std::logic_error("Interpreter::getNextCommand: Invalid command input '" + _command + '\'');
    if (_is.eof())
        throw std::logic_error("Terminated");
}

inline void Interpreter::getNextWord(void)
{
    _is >> _word;
    if (_is.fail())
        throw std::logic_error("Interpreter::getNextWord: Invalid command input '" + _command + '\'');
}

inline bool Interpreter::getNextWordNoThrow(void) noexcept
{
    _is >> _word;
    return (!_is.fail());
}

template<typename As>
inline As Interpreter::getNextWordAs(const char * const what)
{
    As value;

    _is >> value;
    if (_is.fail())
        throw std::logic_error(what);
    return value;
}

inline const Interpreter::NodeHolder &Interpreter::getNode(const std::string &name) const
{
    auto it = _map.find(name);

    if (it == _map.end())
        throw std::logic_error("Interpreter::getNode: Unknown node '" + std::string(name) + '\'');
    return it->second;
}
