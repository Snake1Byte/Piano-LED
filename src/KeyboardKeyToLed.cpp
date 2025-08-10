#include <Arduino.h>
#include "KeyboardKeyToLed.h"
#include "NoteEvent.h"
#include <functional>

std::vector<NeoPixelColor> KeyboardKeyToLed::HandleNoteOn(uint8_t note, uint8_t velocity)
{
    std::vector<NeoPixelColor> neoPixelColors;

    NoteEvent noteEvent(note, velocity, NoteEvent::MidiCommandCode::NoteOn);
    for (auto &strip : PianoLedConfig::globalConfig.strips)
    {
        NeoPixelColor neoPixelColor = Handle(noteEvent, strip);

        strip.litLedsTable[neoPixelColor]++;

        if (strip.litLedsTable[neoPixelColor] > 1)
        {
            continue;
        }

        neoPixelColors.push_back(neoPixelColor);
    }

    return neoPixelColors;
}

std::vector<NeoPixelColor> KeyboardKeyToLed::HandleNoteOff(uint8_t note, uint8_t velocity)
{
    std::vector<NeoPixelColor> neoPixelColors;

    NoteEvent noteEvent(note, velocity, NoteEvent::MidiCommandCode::NoteOff);
    for (auto &strip : PianoLedConfig::globalConfig.strips)
    {
        NeoPixelColor neoPixelColor = Handle(noteEvent, strip);

        if (strip.litLedsTable[neoPixelColor] == 0)
        {
            continue; // No LEDs lit for this color
        }

        strip.litLedsTable[neoPixelColor]--;

        if (strip.litLedsTable[neoPixelColor] > 0)
        {
            continue;
        }

        neoPixelColors.push_back(neoPixelColor);
    }

    return neoPixelColors;
}

NeoPixelColor KeyboardKeyToLed::Handle(const NoteEvent &noteEvent, PianoLedStrip &strip)
{
    int led = static_cast<int>(std::round((noteEvent.noteNumber - lowestKeyOffset) * strip.stripToPianoLengthScale));
    int finalLed = led;

    switch (strip.stripOrientation)
    {
    case PianoLedStrip::StripOrientation::LeftToRight:
    {
        break;
    }
    case PianoLedStrip::StripOrientation::RightToLeft:
    {
        finalLed = strip.totalLeds - led - 1;
        break;
    }
    case PianoLedStrip::StripOrientation::StackedLeftToRight:
    {
        int topOrBottomStackLtr = led % 2;
        if (topOrBottomStackLtr == 0)
        {
            finalLed = led / 2;
        }
        else
        {
            finalLed = strip.totalLeds - led / 2 - 1;
        }
        break;
    }
    case PianoLedStrip::StripOrientation::StackedRightToLeft:
    {
        // TODO
        break;
    }
    }

    int stripNumber = 0;
    for (size_t i = 0; i < PianoLedConfig::globalConfig.strips.size(); ++i)
    {
        if (PianoLedConfig::globalConfig.strips[i] == strip)
        {
            stripNumber = i;
            break;
        }
    }

    if (noteEvent.commandCode == NoteEvent::MidiCommandCode::NoteOff || noteEvent.velocity == 0)
    {
        return NeoPixelColor(stripNumber, finalLed, PianoLedConfig::globalConfig.noteOffColor, PianoLedConfig::globalConfig.noteOffColorBrightness);
    }

    LedColor color(0, 0, 0);
    switch (PianoLedConfig::globalConfig.colorLayout)
    {
    case PianoLedConfig::LedStripColorLayout::VelocityBased:
        color = GradientColorMapping::Map(noteEvent.velocity, 128, PianoLedConfig::globalConfig.colorCurve, PianoLedConfig::globalConfig.colorPalette);
        break;
    case PianoLedConfig::LedStripColorLayout::NoteBased:
        color = GradientColorMapping::Map(led, strip.totalLeds, PianoLedConfig::globalConfig.colorCurve, PianoLedConfig::globalConfig.colorPalette);
        break;
    }

    return NeoPixelColor(stripNumber, finalLed, color, 255);
}
