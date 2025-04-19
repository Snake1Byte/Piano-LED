#include <Arduino.h>
#include <ArduinoJson.h>

#include <Control_Surface.h>
#include <MIDI_Interfaces/USBHostMIDI_Interface.hpp>
#include <vector>

#include "PianoToWled.h"
#include "NoteEvent.h"
#include "PianoLedStrip.h"
#include "NeoPixelColor.h"
#include "GradientColorMapping.h"
#include "LedColor.h"

void InitializeWled();
void ShutdownWled();
void ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel);
void BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness);
boolean VerifyChannel(Channel *channel);

USBHost usb;
USBHub hub (usb);
#define BUFFER_SIZE 512
GenericUSBMIDI_Interface<USBHostMIDIBackend<BUFFER_SIZE>> hostmidi{usb};
USBMIDI_Interface devicemidi;
BidirectionalMIDI_Pipe p;
static const std::vector<uint8_t> allChannels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static boolean hostConnected = false;

// ==================== CUSTOMIZE HERE ====================
#define WLEDSERIAL Serial1 // Serial port for WLED

// "strips" is a vector - you can have multiple LED strips connected to your ESP32. 
// For each strip connected, add a new entry to this vector
std::vector<PianoLedStrip> strips = {                   
    PianoLedStrip(
        // ledsPerSegment: this is important for segmentConnectionMethod below. Every segment of your strip must be of equal length
        74,          
        // segmentCount: also important for segmentConnectionMethod below 
        2,              
        // ledsPerMeter of the entire strip                                 
        60,                                   
        // wledSegmentOffset - offset for the segment in WLED           
        0,            
        // stripToPianoLengthScale - this is important: scale factor for the strip to match the piano length. You need to play around with this value until the addressed LEDs match the height of the played piano keys. For me, 1.68 works well                                   
        1.68,                                            
        // segmentConnectionMethod - how the segments are connected - see picture below
        // Possible values are Parallal, Serial (if the strip is not stacked on top of each other) or None (if you done use multiple segments for the strip)
        PianoLedStrip::SegmentConnectionMethod::Parallel 
    )
};

// Color palette for the gradient mapping 
std::vector<LedColor> colorPalette = {LedColor::Blue, LedColor::Red};

// Color layout strategy: VelocityBased or NoteBased.
// Velocity Based -> the quieter the note, the closer to the first color of the color palette we get
// Note Based -> the lower the note, the closer to the first color of the color palette we get
PianoToWled::LedStripColorLayout colorLayout = PianoToWled::LedStripColorLayout::VelocityBased;

// Color curve function for mapping velocity/note to color. See gradientcolormapping.h for available functions.
std::function<double(double)> colorCurve = GradientColorMapping::Linear;

// Color for note off event
LedColor noteOffColor = LedColor(255, 255, 255);                                                

// Brightness for note off color
int noteOffColorBrightness = 8; 

// MIDI channels to listen to. Use "allChannels" to listen to all channels. My piano (The Yamaha NU1X) for example uses channels 1 and 2 for the piano keys.
std::vector<uint8_t> midiChannelsToListen = {1, 2};

// Here, you can optionally add which note is the lowest note of your piano. This is used to calculate the offset for the LED strip.
// The default is A0, which is the lowest note of a piano. If you want to use a different note, you can do so here.
std::string lowestKey = "A0"; // Lowest note of the piano
// ========================================================

PianoToWled pianoToWled(strips, lowestKey);

struct WledMidiCallbacks : FineGrainedMIDI_Callbacks<WledMidiCallbacks>
{
    void onNoteOn(Channel channel, uint8_t note, uint8_t velocity, Cable cable)
    {
        if (!VerifyChannel(&channel))
        {
            return;
        }

        digitalWrite(LED_BUILTIN, HIGH);

        auto neopixelColors = pianoToWled.HandleNoteOn(note, velocity);
        ChangeIndividualLedColors(neopixelColors);
    }

    void onNoteOff(Channel channel, uint8_t note, uint8_t velocity, Cable cable)
    {
        if (!VerifyChannel(&channel))
        {
            return;
        }

        digitalWrite(LED_BUILTIN, LOW);

        auto neopixelColors = pianoToWled.HandleNoteOff(note, velocity);
        ChangeIndividualLedColors(neopixelColors);
    }

    
    void onControlChange(Channel channel, uint8_t controller, uint8_t value, Cable cable)
    {
        if (!VerifyChannel(&channel))
        {
            return;
        }

        digitalWrite(LED_BUILTIN, LOW);

        // CC AllNotesOff
        if (controller == 123)
        {
            for (auto &strip : strips)
            {
                for (int i = 0; i < strip.segmentCount; ++i)
                {
                    strip.litLedsTable.clear();
                    BulkChangeLedColors(0, strip.ledsPerSegment, i + strip.wledSegmentOffset, pianoToWled.noteOffColor, pianoToWled.noteOffColorBrightness);
                }
            }
        }
    }

} callbacks;

void setup()
{
    // Wait 1.5 seconds before turning on USB Host. If connected USB devices
    // use too much power, Teensy at least completes USB enumeration, which
    // makes isolating the power issue easier.
    delay(1500);

    WLEDSERIAL.begin(115200);
    Serial.begin(115200);
    usb.begin();
    hostmidi.setCallbacks(callbacks);
    devicemidi.setCallbacks(callbacks);
    hostmidi | p | devicemidi;
    pinMode(LED_BUILTIN, OUTPUT);

    // TODO: On / Off toggles

    pianoToWled.colorCurve = colorCurve;
    pianoToWled.colorLayout = colorLayout;
    pianoToWled.colorPalette = colorPalette;
    pianoToWled.noteOffColor = noteOffColor;
    pianoToWled.noteOffColorBrightness = noteOffColorBrightness;

    InitializeWled();
}

void loop()
{
    if (hostmidi.backend.backend && !hostConnected) {
        hostConnected = true;
        InitializeWled();
    }
    else if (!hostmidi.backend.backend && hostConnected)
    {
        hostConnected = false;
        ShutdownWled();
    }
    if (!hostConnected) {
        return;
    }
    hostmidi.update();
    devicemidi.update();
}

void InitializeWled()
{
    Serial1.println("{on:true,bri:128}");
    for (auto &strip : strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            BulkChangeLedColors(0, strip.ledsPerSegment, i + strip.wledSegmentOffset, LedColor(0, 0, 0), 0);
        }
    }

    for (auto &strip : strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            for (int j = 0; j < strip.ledsPerSegment; ++j)
            {
                ChangeIndividualLedColors({NeoPixelColor(j, i, noteOffColor, noteOffColorBrightness)});
                delay(20);
            }
        }
    }
}

void ShutdownWled()
{
    Serial1.println("{on:true,bri:128}");
    for (auto &strip : strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            BulkChangeLedColors(0, strip.ledsPerSegment, i + strip.wledSegmentOffset, noteOffColor, noteOffColorBrightness);
        }
    }

    for (auto &strip : strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            for (int j = 0; j < strip.ledsPerSegment; ++j)
            {
                ChangeIndividualLedColors({NeoPixelColor(j, i, LedColor(0, 0, 0), 0)});
                delay(20);
            }
        }
    }
    Serial1.println("{on:false}");
}

void ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel)
{
    JsonDocument doc;

    JsonArray seg = doc["seg"].to<JsonArray>();

    for (const auto &color : colorsPerPixel)
    {
        JsonObject segment = seg.add<JsonObject>();
        segment["id"] = color.segmentNumber;

        JsonArray i = segment["i"].to<JsonArray>();
        i.add(color.ledNumber);
        i.add(color.hexColor);

        segment["bri"] = color.brightness;
    }

    String json;
    serializeJson(doc, json);
    // Serial.println(json);
    Serial1.println(json);
}

void BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness)
{
    JsonDocument doc;

    JsonArray seg = doc["seg"].to<JsonArray>();

    JsonObject segment = seg.add<JsonObject>();
    segment["id"] = segmentNumber;

    JsonArray i = segment["i"].to<JsonArray>();
    i.add(startLed);
    i.add(endLed);
    JsonArray colorArray = i.add<JsonArray>();

    colorArray.add(color.r);
    colorArray.add(color.g);
    colorArray.add(color.b);

    segment["bri"] = brightness;

    String json;
    serializeJson(doc, json);
    // Serial.println(json);
    Serial1.println(json);
}

boolean VerifyChannel(Channel *channel)
{
    int channelNumber = channel->getRaw() + 1; // Convert to 1-based channel number
    if (std::find(midiChannelsToListen.begin(), midiChannelsToListen.end(), channelNumber) == midiChannelsToListen.end())
    {
        return false;
    }
    return true;
}