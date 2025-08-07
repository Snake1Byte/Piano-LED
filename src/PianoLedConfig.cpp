#include <Arduino.h>
#include "PianoLedConfig.h"

// ==================== CUSTOMIZE HERE ====================
// The data pin that this microcontroller is connected to the LED to. Only used if you opt to use FastLedController instead of WledController (explained further down).
const int ledPin = 23;
PianoLedConfig config = {
    // Serial port for LEDs, needed if connected to an ESP32 over Serial1 that is running WLED 
    .ledSerial = &Serial1,
    // Serial port for LCD (optional)
    .lcdSerial = &Serial2,
    // "strips" is a vector - you can have multiple LED strips connected to your ESP32.
    // For each strip connected, add a new entry to this vector
    .strips = {
        {
            // ledsPerSegment: this is important for segmentConnectionMethod below. Every segment of your strip must be of equal length
            .ledsPerSegment = 74,
            // segmentCount: also important for segmentConnectionMethod below
            .segmentCount = 2,
            // ledsPerMeter of the entire strip
            .ledsPerMeter = 60,
            // wledSegmentOffset - offset for the segment in WLED
            .wledSegmentOffset = 0,
            // stripToPianoLengthScale - this is important: scale factor for the strip to match the piano length. You need to play around with this value until the addressed LEDs match the height of the played piano keys. For me, 1.68 works well
            .stripToPianoLengthScale = 1.68,
            // segmentConnectionMethod - how the segments are connected - see picture below
            // Possible values are Parallal, Serial (if the strip is not stacked on top of each other) or None (if you done use multiple segments for the strip)
            .segmentConnectionMethod = PianoLedStrip::SegmentConnectionMethod::Parallel,
        }},
    // Color palette for the gradient mapping
    .colorPalette = {LedColor::Blue, LedColor::Red},
    // Color layout strategy: VelocityBased or NoteBased.
    // Velocity Based -> the quieter the note, the closer to the first color of the color palette we get
    // Note Based -> the lower the note, the closer to the first color of the color palette we get
    .colorLayout = PianoLedConfig::LedStripColorLayout::VelocityBased,
    // Color curve function for mapping velocity/note to color. See gradientcolormapping.h for available functions.
    .colorCurve = GradientColorMapping::Linear,
    // Color for note off event
    .noteOffColor = LedColor(255, 255, 255),
    // Brightness for note off color
    .noteOffColorBrightness = 6,
    // MIDI channels to listen to. Use "allChannels" to listen to all channels. My piano (The Yamaha NU1X) for example uses channels 1 and 2 for the piano keys.
    .midiChannelsToListen = {1, 2},
    // Here, you can optionally add which note is the lowest note of your piano. This is used to calculate the offset for the LED strip.
    // The default is A0, which is the lowest note of a piano. If you want to use a different note, you can do so here.
    .lowestKey = "A0", // Lowest note of the piano
};
// ========================================================