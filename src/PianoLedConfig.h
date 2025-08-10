#ifndef PIANO_LED_CONFIG_H
#define PIANO_LED_CONFIG_H

#include <vector>
#include <string>
#include <cctype>
#include "NoteEvent.h"
#include "PianoLedStrip.h"
#include "NeoPixelColor.h"
#include "GradientColorMapping.h"
#include "LedColor.h"

struct PianoLedConfig
{
    static int NoteToMidi(const std::string &note);
    static const int maxStrips = 5;
    static const int maxColorPaletteSize = 10;

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

    /**
     * Serial port for the remote ESP32 MCU, used to change the configuration of this program via a webserver running on the ESP32.
     */
    // HardwareSerial *remoteMcuSerial;

    /**
     * This is a vector - you can have up to 5 LED strips connected to your Teensy 4.1. For each strip connected, add a new entry to this vector.
     */
    std::vector<PianoLedStrip> strips;

    /**
     * Color palette for the gradient mapping.
     *
     * E.g. a value of {LedColor::Blue, LedColor::Red} will create
     * a gradient from blue to red, depending on how
     * \ref PianoLedConfig::colorLayout is configured.
     *
     * @see PianoLedConfig::colorLayout
     */
    std::vector<LedColor> colorPalette;

    /**
     * Color layout strategy: VelocityBased or NoteBased.
     * Velocity Based -> the quieter the note, the closer to the first color of the color palette we get.
     * Note Based -> the lower the note, the closer to the first color of the color palette we get.
     */
    LedStripColorLayout colorLayout;

    /**
     * Color curve function for mapping velocity/note to color.
     * See the class \ref GradientColorMapping for available functions.
     */
    std::function<double(double)> colorCurve;

    /**
     * Color for note off event.
     * This color will be used when a note is released or not played at all.
     */
    LedColor noteOffColor;

    /**
     * Brightness for note off color.
     * This value will be used to set the brightness of the note off color. Useful when the note off color is not black/off.
     */
    int noteOffColorBrightness;

    /**
     * MIDI channels to listen to.
     * Set this to the static field "allChannels" to listen to all channels.
     * A Yamaha NU1X for example uses channels 1 and 2 for the piano keys.
     */
    std::vector<uint8_t> midiChannelsToListen;

    /**
     * Here, you can optionally add which note is the lowest note of your piano.
     * This is used to calculate the offset for the LED strip.
     * The default is A0, which is the lowest note of a piano. If you want to use a different note, you can do so here.
     */
    std::string lowestKey;

    static const std::vector<uint8_t> allChannels;
    static PianoLedConfig globalConfig;
};

#endif
