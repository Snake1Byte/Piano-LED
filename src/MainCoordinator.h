#ifndef MAINCOORDINATOR_H
#define MAINCOORDINATOR_H

#include "FastLedController.h"
#include "MidiHostManager.h"
#include "KeyboardKeyToLed.h"
#include "ConfigManager.h"

class MainCoordinator
{
public:
    MainCoordinator();

    void begin();
    void loop();

private:
    MidiHostManager midiHostManager;
    ConfigManager configManager;
    KeyboardKeyToLed keyboardKeyToLed;
    FastLedController ledController;
};

#endif // MAINCOORDINATOR_H