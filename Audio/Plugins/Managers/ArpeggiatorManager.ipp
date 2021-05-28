/**
 * @file ArpeggiatorManager.ipp
 * @brief ArpeggiatorManager implementation
 *
 * @author Pierre V
 * @date 2021-05-21
 */

inline void Audio::ArpeggiatorManager::feedNotes(const NoteEvents &notes) noexcept
{
    // std::cout << "ARP> SEND NOTES " << notes.size() << std::endl;
    const Key lastKey = _cache.actives.size() ? _cache.actives[_cache.noteIndex] : static_cast<Key>(128u);
    bool keyChanged { false };

    for (const auto &note : notes) {
        const auto key = note.key;
        // std::cout << "-" << static_cast<std::size_t>(note.key) << ": " << static_cast<std::size_t>(note.type) << " - " << note.sampleOffset << std::endl;
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            if (const auto it = _cache.actives.find(key); it == _cache.actives.end()) {
                // if (const auto itPending = _cache.pending.find(key); itPending != _cache.pending.end() && _cache.actives.size() >= _cache.noteCount) {
                if (_cache.actives.size() >= _cache.noteCount) {
                    _cache.pending.push(key);
                } else  {
                    _cache.actives.push(key);
                    keyChanged = true;
                }
            }
             else if (const auto itPending = _cache.pending.find(key); itPending == _cache.pending.end()) {
                _cache.pending.push(key);
            }
            break;
        }
        case NoteEvent::EventType::Off:
        {
            _cache.offs.push(note.key);
            break;
        }
        default:
            break;
        }
    }

    if (keyChanged) {
        setNoteIndex(lastKey);
    }
}

inline void Audio::ArpeggiatorManager::generateNotes(NoteEvents &notes, const ArpMode mode, const NoteType noteType) noexcept
{
    UNUSED(notes);
    UNUSED(mode);
    UNUSED(noteType);

    static auto Frames = 0u;

    const Beat syncBeat = MakeBeat(1u, noteType);
    const NoteIndex activesSize = _cache.actives.size();
    const NoteIndex pendingSize = _cache.pending.size();

    NoteIndex realNoteCount = _cache.noteCount < activesSize ? _cache.noteCount : activesSize;
    // auto nextNoteIndex = getNextNoteIndex(realNoteCount);
    // auto nextNoteKey = _cache.actives[nextNoteIndex];

    // std::cout << "=========" << Frames << "=========" << std::endl;
    // std::cout << "_cache.noteCount " << _cache.noteCount << std::endl;
    // std::cout << "activesSize " << activesSize << std::endl;
    // std::cout << "pendingSize " << pendingSize << std::endl;
    // std::cout << "realNoteCount " << realNoteCount << std::endl;
    // std::cout << "syncBeat " << syncBeat << std::endl;
    // std::cout << std::endl;
    setMode(mode, std::max(static_cast<NoteIndex>(realNoteCount + pendingSize), _cache.noteCount));
    _cache.hasEnded = !realNoteCount;
    while (_cache.nextBeat < _cache.range.from)
        _cache.nextBeat += syncBeat;
    for (; _cache.nextBeat < _cache.range.to; _cache.nextBeat += syncBeat) {
        if (!realNoteCount)
            continue;
        // auto sampleOffset =
        auto key = _cache.actives[_cache.noteIndex];

        // Generate off event
        if (_cache.hasStart) {
            // std::cout << " - " << static_cast<std::size_t>(key) << ": OFF @ " << _cache.noteIndex << " // " << static_cast<std::size_t>(_cache.actives.size()) << ":" << static_cast<std::size_t>(_cache.pending.size()) << ":" << static_cast<std::size_t>(_cache.offs.size()) << " #" << Frames << " : " << _cache.nextBeat << std::endl;
            // std::cout << " - " << _cache.noteIndex << " #" << Frames << " : " << _cache.nextBeat << std::endl;
            notes.push(
                NoteEvent {
                    NoteEvent::EventType::Off,
                    key,
                    65535u,
                    0u,
                    0u
                }
            );
            updateOffNotes(realNoteCount);
            if (!realNoteCount) {
                _cache.noteIndex = getNextNoteIndex(1u);
                _cache.hasEnded = true;
            } else {
                _cache.noteIndex = getNextNoteIndex(realNoteCount);
                _cache.hasStart = false;
                key = _cache.actives[_cache.noteIndex];
            }

        }

        // Generate on event
        if (!_cache.hasEnded) {
            if (realNoteCount) {
                // std::cout << " - " << static_cast<std::size_t>(key) << ": ON @ " << _cache.noteIndex << " // " << static_cast<std::size_t>(_cache.actives.size()) << ":" << static_cast<std::size_t>(_cache.pending.size()) << ":" << static_cast<std::size_t>(_cache.offs.size()) << " #" << Frames << " : " << _cache.nextBeat << std::endl;
                // std::cout << " - " << _cache.noteIndex << " #" << Frames << " : " << _cache.nextBeat << std::endl;
                notes.push(
                    NoteEvent {
                        NoteEvent::EventType::On,
                        key,
                        65535u,
                        0u,
                        0u
                    }
                );
                _cache.hasStart = true;
            }
        }
    }
    ++Frames;
}

inline bool Audio::ArpeggiatorManager::updateOffNotes(NoteIndex &realNoteCount) noexcept
{
    if (_cache.offs.size()) {
        // _cache.noteIndex = getNextNoteIndex(realNoteCount);

        // Delete note in actives if its present in offs (if true -> actives.size -= 1)
        auto itOffs = std::remove_if(_cache.offs.begin(), _cache.offs.end(),
            [this, &realNoteCount](const auto key) {
                if (auto it = _cache.actives.find(key); it != _cache.actives.end()) {
                    // std::cout << " -- removeActive " << static_cast<std::size_t>(key) << std::endl;
                    --realNoteCount;
                    _cache.actives.erase(it);
                    return true;
                } else if (auto it = _cache.pending.find(key); it != _cache.pending.end()) {
                    // std::cout << " -- removePending " << static_cast<std::size_t>(key) << std::endl;
                    _cache.pending.erase(it);
                    return true;
                }
                return false;
            }
        );
        if (itOffs != _cache.offs.end()) {
            // std::cout << " \\__removedOffs " << static_cast<std::uint32_t>(std::distance(itOffs, _cache.offs.end())) << std::endl;
            _cache.offs.erase(itOffs, _cache.offs.end());
        }

        // std::cout << " realNoteCount " << static_cast<std::size_t>(realNoteCount) << std::endl;
        if (const auto noteAddedCount = updateActivesNotes(_cache.noteCount - realNoteCount); noteAddedCount) {
            realNoteCount += noteAddedCount;
            // std::cout << " \\__update " << static_cast<std::size_t>(noteAddedCount) << std::endl;
        }
        // std::cout << " \\__update2 " << static_cast<std::size_t>(_cache.actives.size()) << ":" << static_cast<std::size_t>(_cache.pending.size()) << ":" << static_cast<std::size_t>(_cache.offs.size()) << std::endl;
        return true;
    } else
        return false;
}

inline Audio::Key Audio::ArpeggiatorManager::incrementNoteIndex(const NoteIndex maxIndex) noexcept
{
    _cache.noteIndex = getNextNoteIndex(maxIndex);
    return _cache.actives[_cache.noteIndex];
}

inline bool Audio::ArpeggiatorManager::deleteNote(const Key key) noexcept
{
    if (auto itOff = _cache.offs.find(key); itOff != _cache.offs.end()) {
        _cache.offs.erase(itOff);
        if (auto itActive = _cache.actives.find(key); itActive != _cache.actives.end()) {
            _cache.actives.erase(itActive);
            return true;
        } else {
            return false;
        }
    }
    return false;
}

inline Audio::ArpeggiatorManager::NoteIndex Audio::ArpeggiatorManager::updateActivesNotes(const NoteIndex activesAvailableSlots) noexcept
{
    const uint8_t availableSlots = std::min(activesAvailableSlots, _cache.pending.size());
    for (auto i = 0u; i < availableSlots; ++i) {
        _cache.actives.push(*_cache.pending.begin());
        _cache.pending.erase(_cache.pending.begin());
    }
    return availableSlots;
}

inline void Audio::ArpeggiatorManager::setNoteIndex(const Key key) noexcept
{
    if (_cache.actives[_cache.noteIndex] == key)
        return;
    if (const auto it = _cache.actives.find(key); it != _cache.actives.end()) {
        _cache.noteIndex = static_cast<NoteIndex>(std::distance(_cache.actives.begin(), it));
    }
}

inline void Audio::ArpeggiatorManager::setMode(const ArpMode mode, const NoteIndex noteCount) noexcept
{
    if (_cache.mode == mode)
        return;
    _cache.mode = mode;
    switch (_cache.mode) {
    case ArpMode::Up:
        _cache.noteIndex = 0u;
        break;
    case ArpMode::Down:
        _cache.noteIndex = noteCount - 1;
        break;
    case ArpMode::UpDown:
        _cache.noteIndex = 0u;
        _cache.rampUp = true;
        break;
    case ArpMode::DownUp:
        _cache.noteIndex = noteCount - 1;
        _cache.rampUp = true;
        break;

    default:
        break;
    }
}

inline Audio::ArpeggiatorManager::NoteIndex Audio::ArpeggiatorManager::getNextNoteIndex(const NoteIndex maxIndex) noexcept
{
    switch (_cache.mode) {
    case ArpMode::Up:
    {
        if (const NoteIndex nextIndex = _cache.noteIndex + 1; nextIndex >= maxIndex)
            return 0u;
        else
            return nextIndex;
    }
    case ArpMode::Down:
    {
        if (!_cache.noteIndex)
            return maxIndex - 1;
        else
            return _cache.noteIndex - 1;
    }
    case ArpMode::UpDown:
    {
        if (_cache.rampUp) {
            if (const NoteIndex nextIndex = _cache.noteIndex + 1; nextIndex >= maxIndex) {
                _cache.rampUp = false;
                return maxIndex - 1;
            } else {
                return nextIndex;
            }
        } else {
            if (!_cache.noteIndex) {
                _cache.rampUp = true;
                return _cache.noteIndex;
            } else
                return _cache.noteIndex - 1;
        }
        return _cache.noteIndex;
    }
    case ArpMode::DownUp:
    {
        if (!_cache.rampUp) {
            if (const NoteIndex nextIndex = _cache.noteIndex + 1; nextIndex >= maxIndex) {
                _cache.rampUp = true;
                return maxIndex - 1;
            } else {
                return nextIndex;
            }
        } else {
            if (!_cache.noteIndex) {
                _cache.rampUp = false;
                return _cache.noteIndex;
            } else
                return _cache.noteIndex - 1;
        }
        return _cache.noteIndex;
    }
    default:
        return _cache.noteIndex;
    }
}
