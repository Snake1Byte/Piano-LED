#ifndef PIANO_TO_LED_H
#define PIANO_TO_LED_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <cmath>
#include "NeoPixelColor.h"
#include "PianoLedStrip.h"
#include "GradientColorMapping.h"
#include "NoteEvent.h"
#include "PianoLedConfig.h"

class KeyboardKeyToLed
{
public:
    KeyboardKeyToLed()
    {
        lowestKeyOffset = PianoLedConfig::NoteToMidi(PianoLedConfig::globalConfig.lowestKey);
    }

    std::vector<NeoPixelColor> HandleNoteOn(uint8_t note, uint8_t velocity);
    std::vector<NeoPixelColor> HandleNoteOff(uint8_t note, uint8_t velocity);

private:
    int lowestKeyOffset;

    NeoPixelColor Handle(const NoteEvent &noteEvent, PianoLedStrip &strip);
};

#endif
