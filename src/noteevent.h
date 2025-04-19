#ifndef NOTEEVENT_H
#define NOTEEVENT_H

#include <cstdint>

class NoteEvent {
public:
    enum class MidiCommandCode {
        NoteOff,
        NoteOn
    };

    uint8_t noteNumber;
    uint8_t velocity;
    MidiCommandCode commandCode;

    NoteEvent(uint8_t noteNumber = 0, uint8_t velocity = 0, MidiCommandCode commandCode = MidiCommandCode::NoteOff)
        : noteNumber(noteNumber), velocity(velocity), commandCode(commandCode) {}
};

#endif
