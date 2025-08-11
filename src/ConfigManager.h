#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <LittleFS.h>
#include "PianoLedConfig.h"

class ConfigManager
{
public:
    std::function<void(const PianoLedConfig &, bool)> onConfigChanged;

    ConfigManager();

    void begin();
    void loop();

    bool loadConfigFromFile(const char *path, PianoLedConfig &out, bool *configExists);
    bool saveConfigToFile(const char *path, const PianoLedConfig &config);

private:
    LittleFS_Program fs;
    bool fsReady = false;

    void ReadRemoteMCU();
    void writeConfigToStream(Print &out, const PianoLedConfig &config);
    bool parseConfigFromStream(Stream &in, PianoLedConfig &config, uint32_t quietMs = 1500);
    void printConfig(const PianoLedConfig &config);
    bool parseHexColor(const String &v, LedColor &out);
    bool beginFS();
};

#endif // CONFIGMANAGER_H