#include <Arduino.h>
#include "MainCoordinator.h"

MainCoordinator mainCoordinator;

void setup()
{
    mainCoordinator.begin();
}

void loop()
{

    mainCoordinator.loop();
}
