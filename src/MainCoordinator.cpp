#include <Arduino.h>
#include "MainCoordinator.h"
#include "PianoLedConfig.h"

MainCoordinator::MainCoordinator()
    : midiHostManager(), configManager(), keyboardKeyToLed(), ledController()
{
    midiHostManager.onNoteOnCallback = [&](uint8_t note, uint8_t velocity)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        std::vector<NeoPixelColor> colors = keyboardKeyToLed.HandleNoteOn(note, velocity);
        ledController.ChangeIndividualLedColors(colors);
        // {
        //     String output = "Notes started: ";
        //     for (size_t i = 0; i < colors.size(); ++i)
        //     {
        //         output += String(static_cast<int>(colors[i].ledNumber)) + " " + String(colors[i].hexColor.c_str()) + " ";
        //     }
        //     Serial.println(output);
        // }
    };

    midiHostManager.onNoteOffCallback = [&](uint8_t note, uint8_t velocity)
    {
        digitalWrite(LED_BUILTIN, LOW);
        std::vector<NeoPixelColor> colors = keyboardKeyToLed.HandleNoteOff(note, velocity);
        ledController.ChangeIndividualLedColors(colors);
    };

    midiHostManager.onControlChangeCallback = [&](uint8_t cc, uint8_t value)
    {
        digitalWrite(LED_BUILTIN, LOW);

        // CC AllNotesOff
        if (cc == 123)
        {
            for (size_t i = 0; i < PianoLedConfig::globalConfig.strips.size(); ++i)
            {
                auto &strip = PianoLedConfig::globalConfig.strips[i];
                strip.litLedsTable.clear();
                ledController.BulkChangeLedColors(0, strip.totalLeds - 1, i, PianoLedConfig::globalConfig.noteOffColor, PianoLedConfig::globalConfig.noteOffColorBrightness);
            }
        }
    };

    midiHostManager.onHostConnectedCallback = [&](bool connected)
    {
        if (connected)
        {
            ledController.InitializeLeds();
        }
        else
        {
            ledController.ShutdownLeds();
        }
    };

    configManager.onConfigChanged = [&](const PianoLedConfig &newConfig, bool firstTimeSetup)
    {
        if (!firstTimeSetup)
            ledController.ShutdownLeds();
        PianoLedConfig::globalConfig = newConfig;
        ledController = FastLedController();
        keyboardKeyToLed = KeyboardKeyToLed();
        if (!firstTimeSetup)
            ledController.InitializeLeds();
    };
}

void MainCoordinator::begin()
{
    // Wait 1.5 seconds before turning on USB Host. If connected USB devices
    // use too much power, Teensy at least completes USB enumeration, which
    // makes isolating the power issue easier.
    delay(1500);

    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    configManager.begin();
    midiHostManager.begin();
}

void MainCoordinator::loop()
{
    midiHostManager.loop();
    configManager.loop();
}