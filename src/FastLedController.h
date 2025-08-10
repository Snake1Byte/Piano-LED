#ifndef FAST_LED_CONTROLLER_H
#define FAST_LED_CONTROLLER_H

#define FASTLED_ALLOW_INTERRUPTS 0 // Fixes LED flickering issues
#include <FastLED.h>
#include "PianoLedConfig.h"
#include "LedController.h"

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
    void BulkChangeLedColors(int startLed, int endLed, int stripNumber, const LedColor &color, int brightness) override;

private:
    std::vector<std::vector<CRGB>> strips;
    void InitializeFastLed();
};

#endif