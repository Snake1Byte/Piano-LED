#ifndef PIANO_LED_CONFIG_H
#define PIANO_LED_CONFIG_H

#include <vector>
#include "NoteEvent.h"
#include "PianoLedStrip.h"
#include "NeoPixelColor.h"
#include "GradientColorMapping.h"
#include "LedColor.h"

struct PianoLedConfig
{
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

    HardwareSerial *ledSerial;
    HardwareSerial *lcdSerial;
    std::vector<PianoLedStrip> strips;
    std::vector<LedColor> colorPalette;
    LedStripColorLayout colorLayout;
    std::function<double(double)> colorCurve;
    LedColor noteOffColor;
    int noteOffColorBrightness;
    std::vector<uint8_t> midiChannelsToListen;
    std::string lowestKey;
};


extern const int ledPin;
extern PianoLedConfig config;

#endif
