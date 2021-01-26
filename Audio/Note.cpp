/**
 * @ Author: Pierre Veysseyre
 * @ Description: Note.cpp
 */

#include <ostream>
#include "Note.hpp"

using namespace Audio;

const std::vector<const char *> Note::NoteNamesSharp = {
    "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"
};

std::ostream &operator<<(std::ostream &out, const Note &note) {
    const auto id = note.key - 21;

    return out << Note::NoteNamesSharp[(id) % 12] << id / 12;
}

std::ostream &operator<<(std::ostream &out, const NoteEvent &note) {
    const auto id = note.key - 21;

    return out << Note::NoteNamesSharp[(id) % 12] << id / 12;
}
