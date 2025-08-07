#include <Arduino.h>
#include "PianoToLed.h"
#include "NoteEvent.h"
#include <regex>
#include <functional>

std::vector<NeoPixelColor> PianoToLed::HandleNoteOn(uint8_t note, uint8_t velocity)
{
    std::vector<NeoPixelColor> neoPixelColors;

    NoteEvent noteEvent(note, velocity, NoteEvent::MidiCommandCode::NoteOn);
    for (auto &strip : config.strips)
    {
        NeoPixelColor neoPixelColor = KeyboardKeyToLed(noteEvent, strip);

        strip.litLedsTable[neoPixelColor]++;

        if (strip.litLedsTable[neoPixelColor] > 1)
        {
            continue;
        }

        neoPixelColors.push_back(neoPixelColor);
    }

    return neoPixelColors;
}

std::vector<NeoPixelColor> PianoToLed::HandleNoteOff(uint8_t note, uint8_t velocity)
{
    std::vector<NeoPixelColor> neoPixelColors;

    NoteEvent noteEvent(note, velocity, NoteEvent::MidiCommandCode::NoteOff);
    for (auto &strip : config.strips)
    {
        NeoPixelColor neoPixelColor = KeyboardKeyToLed(noteEvent, strip);

        strip.litLedsTable[neoPixelColor]--;

        if (strip.litLedsTable[neoPixelColor] > 0)
        {
            continue;
        }

        neoPixelColors.push_back(neoPixelColor);
    }

    return neoPixelColors;
}

NeoPixelColor PianoToLed::KeyboardKeyToLed(const NoteEvent &noteEvent, PianoLedStrip &strip)
{
    int led = static_cast<int>(std::round((noteEvent.noteNumber - lowestKeyOffset) * strip.stripToPianoLengthScale));
    int finalLed = led;
    int segmentNumber = 0;

    switch (strip.segmentConnectionMethod)
    {
    case PianoLedStrip::SegmentConnectionMethod::None:
        segmentNumber = strip.wledSegmentOffset;
        break;
    case PianoLedStrip::SegmentConnectionMethod::Serial:
        segmentNumber = strip.wledSegmentOffset + strip.ledsPerSegment / led;
        finalLed -= strip.ledsPerSegment * (led / strip.ledsPerSegment);
        break;
    case PianoLedStrip::SegmentConnectionMethod::Parallel:
        segmentNumber = strip.wledSegmentOffset + (led % strip.segmentCount);
        finalLed /= strip.segmentCount;
        break;
    }

    if (noteEvent.commandCode == NoteEvent::MidiCommandCode::NoteOff || noteEvent.velocity == 0)
    {
        return NeoPixelColor(finalLed, segmentNumber, config.noteOffColor, config.noteOffColorBrightness);
    }

    LedColor color(0, 0, 0);
    switch (config.colorLayout)
    {
    case PianoLedConfig::LedStripColorLayout::VelocityBased:
        color = GradientColorMapping::Map(noteEvent.velocity, 128, config.colorCurve, config.colorPalette);
        break;
    case PianoLedConfig::LedStripColorLayout::NoteBased:
        int parallelConnectionMethodMult = strip.segmentConnectionMethod == PianoLedStrip::SegmentConnectionMethod::Parallel ? strip.segmentCount : 1;
        double finalLedsPerMeter = strip.ledsPerMeter * parallelConnectionMethodMult;
        color = GradientColorMapping::Map(led, finalLedsPerMeter, config.colorCurve, config.colorPalette);
        break;
    }

    return NeoPixelColor(finalLed, segmentNumber, color, 255);
}

int PianoToLed::NoteToMidi(const std::string &note)
{
    static const std::unordered_map<std::string, int> noteOffsets = {
        {"C", 0}, {"C#", 1}, {"D", 2}, {"D#", 3}, {"E", 4}, {"F", 5}, {"F#", 6}, {"G", 7}, {"G#", 8}, {"A", 9}, {"A#", 10}, {"B", 11}};

    static const std::unordered_map<std::string, std::string> flatToSharp = {
        {"Db", "C#"}, {"D♭", "C#"}, {"Eb", "D#"}, {"E♭", "D#"}, {"Fb", "E"}, {"F♭", "E"}, {"Gb", "F#"}, {"G♭", "F#"}, {"Ab", "G#"}, {"A♭", "G#"}, {"Bb", "A#"}, {"B♭", "A#"}, {"Cb", "B"}, {"C♭", "B"}};

    std::regex re("^([A-Ga-g][#♯b♭]?)(\\d+)$");
    std::smatch match;

    if (!std::regex_match(note, match, re))
    {
        return -1; // Invalid note format
    }

    std::string noteName = match[1].str();
    int octave = std::stoi(match[2].str());

    if (flatToSharp.count(noteName))
    {
        noteName = flatToSharp.at(noteName);
    }

    if (!noteOffsets.count(noteName))
    {
        return -1; // Invalid note format
    }

    return noteOffsets.at(noteName) + ((octave + 1) * 12);
}