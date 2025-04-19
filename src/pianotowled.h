// PianoToWled.h
#ifndef PIANO_TO_WLED_H
#define PIANO_TO_WLED_H

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <cmath>
#include "NeoPixelColor.h"
#include "PianoLedStrip.h"
#include "GradientColorMapping.h"
#include "NoteEvent.h"

class PianoToWled
{
public:
    /**
     * @enum LedStripColorLayout
     * @brief Defines the color layout strategy for the LED strip.
     */
    enum class LedStripColorLayout
    {
        /**
         * The color of the LED will be based on the velocity of the note.
         */

        VelocityBased,
        /**
         * The color of the LED will be determined by what note is being played.
         */
        NoteBased
    };

    PianoToWled(std::vector<PianoLedStrip> &strips, const std::string &lowestKey = "A0")
        : strips(strips)
    {
        lowestKeyOffset = NoteToMidi(lowestKey);
    }

    LedStripColorLayout colorLayout = LedStripColorLayout::VelocityBased;
    std::function<double(double)> colorCurve = GradientColorMapping::Linear;
    std::vector<LedColor> colorPalette = {LedColor::Blue};
    LedColor noteOffColor = LedColor(0, 0, 0); // Default to black
    int noteOffColorBrightness = 0;            // Default to 0 (off)

    std::vector<NeoPixelColor> HandleNoteOn(uint8_t note, uint8_t velocity);
    std::vector<NeoPixelColor> HandleNoteOff(uint8_t note, uint8_t velocity);

private:
    std::vector<PianoLedStrip> &strips;
    int lowestKeyOffset;

    NeoPixelColor KeyboardKeyToLed(const NoteEvent &noteEvent, PianoLedStrip &strip, LedStripColorLayout layout);
    int NoteToMidi(const std::string &note);
};

#endif
