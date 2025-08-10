#include <Arduino.h>
#include "PianoLedConfig.h"
#include <regex>

PianoLedConfig PianoLedConfig::globalConfig = {
    .strips = std::vector<PianoLedStrip>{
        PianoLedStrip{
            2,                                                  // ledPin
            148,                                                // totalLeds
            60,                                                 // ledsPerMeter
            1.68,                                               // stripToPianoLengthScale
            PianoLedStrip::StripOrientation::StackedLeftToRight // stripOrientation
        }},
    .colorPalette = {LedColor::Blue, LedColor::Red},
    .colorLayout = PianoLedConfig::LedStripColorLayout::VelocityBased,
    .colorCurve = GradientColorMapping::Linear,
    .noteOffColor = LedColor(255, 255, 255),
    .noteOffColorBrightness = 6,
    .midiChannelsToListen = {1, 2},
    .lowestKey = "A0", // Lowest note of the piano
};

const std::vector<uint8_t> PianoLedConfig::allChannels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};

int PianoLedConfig::NoteToMidi(const std::string &note)
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