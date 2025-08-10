#ifndef PIANO_LED_STRIP_H
#define PIANO_LED_STRIP_H

#include <unordered_map>
#include "NeoPixelColor.h"

struct PianoLedStrip
{
    /**
     * @enum SegmentConnectionMethod
     * @brief Defines how the LED strip above your keyboard are connected to the Teensy 4.1.
     */
    enum class StripOrientation
    {
        /**
         * The strip is connected from left to right -> LED with index 0 is the leftmost LED, LED with index N-1 is the rightmost LED.
         */
        LeftToRight,
        /**
         * The strip is connected from right to left -> LED with index 0 is the rightmost LED, LED with index N-1 is the leftmost LED.
         */
        RightToLeft,
        /**
         * The strip is stacked on top of each other from left to right -> LED with index 0 is the leftmost LED, LED with index (N-1)/2 is the rightmost LED, and LED with index N-1 is the leftmost LED again.
         */
        StackedLeftToRight,
        /**
         * The strip is stacked on top of each other from right to left -> LED with index 0 is the rightmost LED, LED with index (N-1)/2 is the leftmost LED, and LED with index N-1 is the rightmost LED again.
         */
        StackedRightToLeft
    };

    /**
     * What Teensy 4.1 pin this LED strip is connected to. Possible values are pins 2-6.
     */
    int ledPin;

    /**
     * How many LEDs there are in total on this instance of PianoLedStrip.
     */
    int totalLeds;

    /**
     * How many LEDs there are per meter of this instance of PianoLedStrip.
     */
    double ledsPerMeter;

    /**
     * Scale factor for the strip to match the piano length. You need to play around with this value until the addressed LEDs match the height of the played piano keys. Usually, a value of 1.68 works well for a 60 LED per meter strip.
     */
    double stripToPianoLengthScale;

    /**
     * How are the LEDs oriented?
     */
    StripOrientation stripOrientation;

    /**
     * Do not change: used by this program to calculate which LEDs to light up.
     */
    std::unordered_map<NeoPixelColor, int> litLedsTable;

    // Equality operator: identity determined by ledPin
    bool operator==(const PianoLedStrip &other) const
    {
        return ledPin == other.ledPin;
    }
};

namespace std
{
    template <>
    struct hash<PianoLedStrip>
    {
        size_t operator()(PianoLedStrip const &c) const
        {
            return std::hash<int>()(c.ledPin);
        }
    };
}

#endif // PIANO_LED_STRIP_H