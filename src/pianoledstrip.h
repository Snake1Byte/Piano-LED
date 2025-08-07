#ifndef PIANO_LED_STRIP_H
#define PIANO_LED_STRIP_H

#include <unordered_map>
#include "NeoPixelColor.h"

struct PianoLedStrip
{
    enum class SegmentConnectionMethod
    {
        None,
        Serial,
        Parallel
    };

    int ledsPerSegment;
    int segmentCount;
    double ledsPerMeter;
    int wledSegmentOffset;
    /**
     * Scale factor for the strip to match the paino length.
     */
    double stripToPianoLengthScale;
    SegmentConnectionMethod segmentConnectionMethod;

    std::unordered_map<NeoPixelColor, int> litLedsTable;
};

#endif // PIANO_LED_STRIP_H