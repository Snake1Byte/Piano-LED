#include <Arduino.h>
#include <ArduinoJson.h>

#include <Control_Surface.h>
#include <MIDI_Interfaces/USBHostMIDI_Interface.hpp>
#include <vector>

#include "PianoToLed.h"
#include "PianoLedConfig.h"
#include "NoteEvent.h"
#include "PianoLedStrip.h"
#include "NeoPixelColor.h"
#include "GradientColorMapping.h"
#include "LedColor.h"
#include "LedController.h"

boolean VerifyChannel(Channel *channel);

USBHost usb;
USBHub hub(usb);
#define BUFFER_SIZE 512
GenericUSBMIDI_Interface<USBHostMIDIBackend<BUFFER_SIZE>> hostmidi{usb};
USBMIDI_Interface devicemidi;
BidirectionalMIDI_Pipe p;
static const std::vector<uint8_t> allChannels = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static boolean hostConnected = false;

PianoToLed pianoToLed;
// Possible classes: WledController controller(config) or FastLedController controller(config).
// Use the former if you have an ESP32 with WLED connected to this microcontroller's Serial1 pins.
// Use the latter if you want this microcontroller to control the LEDs directly (using the FastLED library).
WledController controller;

struct LedMidiCallbacks : FineGrainedMIDI_Callbacks<LedMidiCallbacks>
{
    void onNoteOn(Channel channel, uint8_t note, uint8_t velocity, Cable cable)
    {
        if (velocity == 0)
        {
            onNoteOff(channel, note, 0, cable);
            return;
        }

        if (!VerifyChannel(&channel))
        {
            return;
        }

        auto neopixelColors = pianoToLed.HandleNoteOn(note, velocity);
        controller.ChangeIndividualLedColors(neopixelColors);
        digitalWrite(LED_BUILTIN, HIGH);
        // LCDSERIAL.print("0");
        // LCDSERIAL.print("1 Note On: " + String(note));
        // LCDSERIAL.print("2 Velocity: " + String(velocity));
    }

    void onNoteOff(Channel channel, uint8_t note, uint8_t velocity, Cable cable)
    {
        if (!VerifyChannel(&channel))
        {
            return;
        }

        auto neopixelColors = pianoToLed.HandleNoteOff(note, velocity);
        controller.ChangeIndividualLedColors(neopixelColors);
        digitalWrite(LED_BUILTIN, LOW);
        // LCDSERIAL.print("2 Velocity: " + String(velocity));
        // LCDSERIAL.print("0");
        // LCDSERIAL.print("1 Note Off: " + String(note));
    }

    void onControlChange(Channel channel, uint8_t cc, uint8_t value, Cable cable)
    {
        if (!VerifyChannel(&channel))
        {
            return;
        }

        digitalWrite(LED_BUILTIN, LOW);

        // CC AllNotesOff
        if (cc == 123)
        {
            for (auto &strip : config.strips)
            {
                for (int i = 0; i < strip.segmentCount; ++i)
                {
                    strip.litLedsTable.clear();
                    controller.BulkChangeLedColors(0, strip.ledsPerSegment, i + strip.wledSegmentOffset, config.noteOffColor, config.noteOffColorBrightness);
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

    config.ledSerial->begin(115200);
    config.lcdSerial->begin(115200);
    Serial.begin(115200);
    usb.begin();
    hostmidi.setCallbacks(callbacks);
    devicemidi.setCallbacks(callbacks);
    hostmidi | p | devicemidi;
    pinMode(LED_BUILTIN, OUTPUT);

    // TODO: On / Off toggles

    // Turn off WLED in case it's on without a host connected
    if (!hostmidi.backend.backend)
    {
        delay(3000);
        Serial1.println("{on:false}");
    }
}

void loop()
{
    if (hostmidi.backend.backend && !hostConnected)
    {
        hostConnected = true;
        controller.InitializeLeds();
    }
    else if (!hostmidi.backend.backend && hostConnected)
    {
        hostConnected = false;
        controller.ShutdownLeds();
    }

    if (!hostConnected)
    {
        return;
    }

    hostmidi.update();
    devicemidi.update();
}

boolean VerifyChannel(Channel *channel)
{
    int channelNumber = channel->getRaw() + 1; // Convert to 1-based channel number
    if (std::find(config.midiChannelsToListen.begin(), config.midiChannelsToListen.end(), channelNumber) == config.midiChannelsToListen.end())
    {
        return false;
    }
    return true;
}