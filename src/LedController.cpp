#include <Arduino.h>
#include <ArduinoJson.h>
#include "PianoToLed.h"
#include "LedController.h"
#include "PianoToLed.h"
#include "PianoLedConfig.h"
#include "NoteEvent.h"
#include "PianoLedStrip.h"
#include "NeoPixelColor.h"
#include "GradientColorMapping.h"
#include "LedColor.h"

WledController::WledController()
{
}

void WledController::InitializeLeds()
{
    config.ledSerial->println("{on:true,bri:128}");
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

void WledController::ShutdownLeds()
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

    config.ledSerial->println("{on:false}");
}

void WledController::ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel)
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
    config.ledSerial->println(json);
}

void WledController::BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness)
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
    config.ledSerial->println(json);
}
