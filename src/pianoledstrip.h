#ifndef PIANO_LED_STRIP_H
#define PIANO_LED_STRIP_H

#include <unordered_map>
#include "NeoPixelColor.h"

class PianoLedStrip {
public:
    enum class SegmentConnectionMethod {
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

    PianoLedStrip(int ledsPerSegment,
                  int segmentCount,
                  double ledsPerMeter,
                  int wledSegmentOffset,
                  double stripToPianoLengthScale,
                  SegmentConnectionMethod segmentConnectionMethod)
        : ledsPerSegment(ledsPerSegment),
          segmentCount(segmentCount),
          ledsPerMeter(ledsPerMeter),
          wledSegmentOffset(wledSegmentOffset),
          stripToPianoLengthScale(stripToPianoLengthScale),
          segmentConnectionMethod(segmentConnectionMethod) {}
};

#endif // PIANO_LED_STRIP_H