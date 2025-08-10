#include <Arduino.h>
#include "FastLedController.h"
#include "PianoLedConfig.h"

void FastLedController::InitializeFastLed()
{
    size_t numStrips = PianoLedConfig::globalConfig.strips.size();
    strips.resize(numStrips);
    delay(1000); // power-up safety delay
    for (size_t i = 0; i < numStrips; ++i)
    {
        strips[i].resize(PianoLedConfig::globalConfig.strips[i].totalLeds);
        switch (PianoLedConfig::globalConfig.strips[i].ledPin)
        {
        case 2:
            FastLED.addLeds<WS2812B, 2, GRB>(strips[i].data(), PianoLedConfig::globalConfig.strips[i].totalLeds);
            break;
        case 3:
            FastLED.addLeds<WS2812B, 3, GRB>(strips[i].data(), PianoLedConfig::globalConfig.strips[i].totalLeds);
            break;
        case 4:
            FastLED.addLeds<WS2812B, 4, GRB>(strips[i].data(), PianoLedConfig::globalConfig.strips[i].totalLeds);
            break;
        case 5:
            FastLED.addLeds<WS2812B, 5, GRB>(strips[i].data(), PianoLedConfig::globalConfig.strips[i].totalLeds);
            break;
        case 6:
            FastLED.addLeds<WS2812B, 6, GRB>(strips[i].data(), PianoLedConfig::globalConfig.strips[i].totalLeds);
        }
    }
}

void FastLedController::InitializeLeds()
{
    for (size_t stripNumber = 0; stripNumber < PianoLedConfig::globalConfig.strips.size(); ++stripNumber)
    {
        for (int i = 0; i < PianoLedConfig::globalConfig.strips[stripNumber].totalLeds; ++i)
        {
            ChangeIndividualLedColors({NeoPixelColor(stripNumber, i, PianoLedConfig::globalConfig.noteOffColor, PianoLedConfig::globalConfig.noteOffColorBrightness)});
            delay(10);
        }
    }
}

void FastLedController::ShutdownLeds()
{
    for (size_t stripNumber = 0; stripNumber < PianoLedConfig::globalConfig.strips.size(); ++stripNumber)
    {
        for (int i = 0; i < PianoLedConfig::globalConfig.strips[stripNumber].totalLeds; ++i)
        {
            ChangeIndividualLedColors({NeoPixelColor(stripNumber, i, LedColor(0, 0, 0), 0)});
            delay(10);
        }
    }
}

void FastLedController::ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel)
{
    for (auto &color : colorsPerPixel)
    {
        CRGB ledColor(color.ledColor.r, color.ledColor.g, color.ledColor.b);
        ledColor.nscale8_video(color.brightness);
        strips[color.stripNumber][color.ledNumber] = ledColor;
    }
    FastLED.show();
}

void FastLedController::BulkChangeLedColors(int startLed, int endLed, int stripNumber, const LedColor &color, int brightness)
{
    CRGB ledColor(color.r, color.g, color.b);
    ledColor.nscale8_video(brightness);

    for (int i = startLed; i < endLed; ++i)
    {
        strips[stripNumber][i] = ledColor;
    }
    FastLED.show();
}
