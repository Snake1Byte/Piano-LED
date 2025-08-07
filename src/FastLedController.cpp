#include <Arduino.h>
#include "FastLedController.h"
#include <FastLED.h>
#include "PianoLedConfig.h"

void FastLedController::InitializeLeds()
{
    for (auto &strip : config.strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            BulkChangeLedColors(0, strip.ledsPerSegment, i + strip.wledSegmentOffset, LedColor(0, 0, 0), 0);
        }
    }

    for (auto &strip : config.strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            for (int j = 0; j < strip.ledsPerSegment; ++j)
            {
                ChangeIndividualLedColors({NeoPixelColor(j, i + strip.wledSegmentOffset, config.noteOffColor, config.noteOffColorBrightness)});
                delay(10);
            }
        }
    }
}

void FastLedController::ShutdownLeds()
{
    for (auto &strip : config.strips)
    {
        strip.litLedsTable.clear();

        for (int i = 0; i < strip.segmentCount; ++i)
        {
            BulkChangeLedColors(0, strip.ledsPerSegment, i + strip.wledSegmentOffset, config.noteOffColor, config.noteOffColorBrightness);
        }
    }

    for (auto &strip : config.strips)
    {
        for (int i = 0; i < strip.segmentCount; ++i)
        {
            for (int j = 0; j < strip.ledsPerSegment; ++j)
            {
                ChangeIndividualLedColors({NeoPixelColor(j, i + strip.wledSegmentOffset, LedColor(0, 0, 0), 0)});
                delay(10);
            }
        }
    }
}

void FastLedController::ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel)
{
    // for (const auto &color : colorsPerPixel)
    // {
    //     JsonObject segment = seg.add<JsonObject>();
    //     segment["id"] = color.segmentNumber;

    //     JsonArray i = segment["i"].to<JsonArray>();
    //     i.add(color.ledNumber);
    //     i.add(color.hexColor);

    //     segment["bri"] = color.brightness;
    // }

    // String json;
    // serializeJson(doc, json);
    // // Serial.println(json);
    // Serial1.println(json);
}

void FastLedController::BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness)
{
    // JsonDocument doc;

    // JsonArray seg = doc["seg"].to<JsonArray>();

    // JsonObject segment = seg.add<JsonObject>();
    // segment["id"] = segmentNumber;

    // JsonArray i = segment["i"].to<JsonArray>();
    // i.add(startLed);
    // i.add(endLed);
    // JsonArray colorArray = i.add<JsonArray>();

    // colorArray.add(color.r);
    // colorArray.add(color.g);
    // colorArray.add(color.b);

    // segment["bri"] = brightness;

    // String json;
    // serializeJson(doc, json);
    // // Serial.println(json);
    // Serial1.println(json);
}

void FastLedController::InitializeFastLed()
{
    int totalLeds = 0;
    for (const auto &strip : config.strips)
    {
        totalLeds += strip.ledsPerSegment * strip.segmentCount;
    }

    leds = new CRGB[totalLeds];

    delay(3000); // power-up safety delay
    FastLED.addLeds<WS2812B, GRB>(leds, totalLeds, (uint8_t)ledPin).setCorrection(TypicalLEDStrip);
}