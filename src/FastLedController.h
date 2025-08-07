#ifndef FAST_LED_CONTROLLER_H
#define FAST_LED_CONTROLLER_H

#include "PianoLedConfig.h"
#include "LedController.h"
#include <FastLED.h>

class FastLedController : public ILedController
{
public:
    FastLedController()
    {
        InitializeFastLed();
    };

    void InitializeLeds() override;
    void ShutdownLeds() override;
    void ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel) override;
    void BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness) override;

private:
    CRGB *leds;
    void InitializeFastLed();
};

#endif