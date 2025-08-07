#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include "PianoToLed.h"
#include "NoteEvent.h"
#include "PianoLedStrip.h"
#include "NeoPixelColor.h"
#include "GradientColorMapping.h"
#include "LedColor.h"

class ILedController
{
public:
    virtual ~ILedController() {}

    virtual void InitializeLeds() = 0;
    virtual void ShutdownLeds() = 0;
    virtual void ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel) = 0;
    virtual void BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness) = 0;
};

class WledController : public ILedController
{
public:
    WledController();

    void InitializeLeds() override;
    void ShutdownLeds() override;
    void ChangeIndividualLedColors(const std::vector<NeoPixelColor> &colorsPerPixel) override;
    void BulkChangeLedColors(int startLed, int endLed, int segmentNumber, const LedColor &color, int brightness) override;
};

#endif
