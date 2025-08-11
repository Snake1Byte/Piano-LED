#include <Arduino.h>
#include "ConfigManager.h"
#include "PianoLedConfig.h"
#include <vector>
#include <string>

ConfigManager::ConfigManager()
    : onConfigChanged(nullptr)
{
}

DMAMEM uint8_t rxBuf[4096];
DMAMEM uint8_t txBuf[4096];

void ConfigManager::begin()
{
    Serial1.begin(115200);
    Serial1.addMemoryForRead(rxBuf, sizeof(rxBuf));  // enlarge RX ring buffer
    Serial1.addMemoryForWrite(txBuf, sizeof(txBuf)); // enlarge TX ring buffer
    beginFS();

    Serial.println("Loading config from file...");
    PianoLedConfig config;
    bool exists = false;
    bool ok = loadConfigFromFile("/config.txt", config, &exists);

    if (!exists)
    {
        Serial.println("Config file not found, creating default config...");
        ok = saveConfigToFile("/config.txt", PianoLedConfig::globalConfig);
        if (ok)
        {
            Serial.println("Default config created successfully.");
        }
        else
        {
            Serial.println("Failed to create default config.");
            return;
        }
    }
    else
    {
        Serial.println(ok ? "Config loaded successfully." : "Failed to load config.");
        if (ok)
        {
            if (onConfigChanged)
                onConfigChanged(config, true);
        }
    }
}

void ConfigManager::loop()
{
    ReadRemoteMCU();
}

void ConfigManager::ReadRemoteMCU()
{
    if (Serial1.available())
    {
        String cmd = Serial1.readStringUntil('\n');
        Serial.println("Received command: " + cmd);
        cmd.trim();
        if (cmd == "Requesting Config")
        {
            Serial.println("Sending following config to remote MCU:");
            printConfig(PianoLedConfig::globalConfig);
            Serial.println();
            writeConfigToStream(Serial1, PianoLedConfig::globalConfig);
        }
        else if (cmd == "Config change")
        {
            PianoLedConfig newConfig;
            parseConfigFromStream(Serial1, newConfig);
            Serial.println("Config change received from remote MCU.");
            saveConfigToFile("/config.txt", newConfig);
            if (onConfigChanged)
            {
                onConfigChanged(newConfig, false);
            }
        }
    }
}

bool ConfigManager::loadConfigFromFile(const char *path, PianoLedConfig &out, bool *configExists)
{
    if (!fsReady)
        return false;

    if (!fs.exists(path))
    {
        *configExists = false;
        return false;
    }
    else
    {
        *configExists = true;
    }

    File f = fs.open(path, FILE_READ);
    if (!f)
    {
        Serial.printf("loadConfigFromFile: open('%s') failed\n", path);
        return false;
    }

    bool ok = parseConfigFromStream(f, out);

    if (ok)
    {
        Serial.println("Config loaded from file successfully:");
        printConfig(out);
        Serial.println();
    }

    f.close();
    return ok;
}

bool ConfigManager::saveConfigToFile(const char *path, const PianoLedConfig &config)
{
    if (!beginFS())
        return false;

    if (fs.exists(path))
        fs.remove(path);
    File f = fs.open(path, FILE_WRITE);
    if (!f)
    {
        Serial.printf("open('%s') failed\n", path);
        return false;
    }

    uint32_t before = f.position();
    writeConfigToStream(f, config);
    f.flush();
    uint32_t written = f.position() - before;
    f.close();

    Serial.printf("saveConfigToFile: wrote %lu bytes to '%s'\n", (unsigned long)written, path);
    bool ok = written > 0;

    if (ok)
    {
        Serial.println("Config saved to file successfully:");
        printConfig(config);
        Serial.println();
    }

    return ok;
}

void ConfigManager::writeConfigToStream(Print &out, const PianoLedConfig &config)
{
    // TODO
    // -- send colorCurve

    out.println("Sending Config");
    for (size_t i = 0; i < config.strips.size(); ++i)
    {
        out.print("strip[");
        out.print(i);
        out.println("]");
        out.print("ledPin = ");
        out.println(config.strips[i].ledPin);
        out.print("totalLeds = ");
        out.println(config.strips[i].totalLeds);
        out.print("ledsPerMeter = ");
        out.println(config.strips[i].ledsPerMeter);
        out.print("stripToPianoLengthScale = ");
        out.println(config.strips[i].stripToPianoLengthScale);
        out.print("stripOrientation = ");
        switch (config.strips[i].stripOrientation)
        {
        case PianoLedStrip::StripOrientation::LeftToRight:
            out.println("LeftToRight");
            break;
        case PianoLedStrip::StripOrientation::RightToLeft:
            out.println("RightToLeft");
            break;
        case PianoLedStrip::StripOrientation::StackedLeftToRight:
            out.println("StackedLeftToRight");
            break;
        case PianoLedStrip::StripOrientation::StackedRightToLeft:
            out.println("StackedRightToLeft");
            break;
        }
    }

    for (size_t i = 0; i < config.colorPalette.size(); ++i)
    {
        auto &c = config.colorPalette[i];
        char buf[8];
        sprintf(buf, "%02X%02X%02X", c.r, c.g, c.b);
        out.print("colorPalette[");
        out.print(i);
        out.print("] = #");
        out.println(buf);
    }

    out.print("colorLayout = ");
    out.println(config.colorLayout == PianoLedConfig::LedStripColorLayout::VelocityBased
                    ? "VelocityBased"
                    : "NoteBased");
    {
        auto &c = config.noteOffColor;
        char buf[8];
        sprintf(buf, "%02X%02X%02X", c.r, c.g, c.b);
        out.print("noteOffColor = #");
        out.println(buf);
    }
    out.print("noteOffColorBrightness = ");
    out.println(config.noteOffColorBrightness);
    out.print("midiChannelsToListen = ");
    for (size_t i = 0; i < config.midiChannelsToListen.size(); ++i)
    {
        if (i)
        {
            out.print(',');
        }
        out.print(config.midiChannelsToListen[i]);
    }
    out.println();
    out.print("lowestKey = ");
    out.println(config.lowestKey.c_str());
    out.println("End of Config");
}

// Example transmission:
// Sending Config
// strip[0]
// ledPin = 2
// totalLeds = 148
// ledsPerMeter = 148
// stripToPianoLengthScale = 1.68
// stripOrientation = StackedLeftToRight
// colorPalette[0] = #0000FF
// colorPalette[1] = #FF0000
// colorLayout = VelocityBased
// noteOffColor = #FFFFFF
// noteOffColorBrightness = 6
// midiChannelsToListen = 1,2
// lowestKey = A0
bool ConfigManager::parseConfigFromStream(Stream &in, PianoLedConfig &config, uint32_t quietMs = 1500)
{
    // TODO color curve

    config.colorPalette.clear();
    config.midiChannelsToListen.clear();
    config.strips.clear();

    int currentStripIndex = -1;
    int colorPaletteIndex = -1;

    in.setTimeout(50);
    unsigned long lastByteMs = millis();
    size_t parsed_lines = 0;
    for (;;)
    {
        ++parsed_lines;
        String line = in.readStringUntil('\n');
        if (line.length() == 0)
        {
            if (millis() - lastByteMs >= quietMs)
                break;
            delay(10);
            continue;
        }

        lastByteMs = millis();
        line.trim();

        if (line == "Sending Config")
            continue;
        if (line == "End of Config")
            break;

        if (line.startsWith("strip["))
        {
            currentStripIndex = line.substring(6, line.indexOf(']')).toInt();
            if (currentStripIndex >= 0 && (int)config.strips.size() <= currentStripIndex && currentStripIndex < PianoLedConfig::maxStrips)
                config.strips.resize(currentStripIndex + 1);
            continue;
        }

        int eq = line.indexOf('=');
        if (eq <= 0)
            continue;

        String k = line.substring(0, eq);
        k.trim();
        String v = line.substring(eq + 1);
        v.trim();

        // Use the explicit index so we don't call back() on an empty vector
        auto setStripField = [&](auto setter)
        {
            if (currentStripIndex >= 0 &&
                currentStripIndex < (int)config.strips.size() && currentStripIndex < PianoLedConfig::maxStrips)
            {
                setter(config.strips[currentStripIndex]);
            }
        };

        if (k == "ledPin")
            setStripField([&](auto &s)
                          { s.ledPin = v.toInt(); });
        else if (k == "totalLeds")
            setStripField([&](auto &s)
                          { s.totalLeds = v.toInt(); });
        else if (k == "ledsPerMeter")
            setStripField([&](auto &s)
                          { s.ledsPerMeter = v.toFloat(); });
        else if (k == "stripToPianoLengthScale")
            setStripField([&](auto &s)
                          { s.stripToPianoLengthScale = v.toFloat(); });
        else if (k == "stripOrientation")
            setStripField([&](auto &s)
                          { if (v == "StackedLeftToRight")
                                    s.stripOrientation = PianoLedStrip::StripOrientation::StackedLeftToRight;
                                else if (v == "StackedRightToLeft")
                                    s.stripOrientation = PianoLedStrip::StripOrientation::StackedRightToLeft;
                                else if (v == "LeftToRight")
                                    s.stripOrientation = PianoLedStrip::StripOrientation::LeftToRight;
                                else if (v == "RightToLeft")
                                    s.stripOrientation = PianoLedStrip::StripOrientation::RightToLeft;
                                else if (v.toInt() >= 0 && v.toInt() < 4) {
                                    s.stripOrientation = static_cast<PianoLedStrip::StripOrientation>(v.toInt());
                                } });
        else if (k.startsWith("colorPalette["))
        {
            colorPaletteIndex = line.substring(13, line.indexOf(']')).toInt();
            if (colorPaletteIndex >= 0 && colorPaletteIndex < PianoLedConfig::maxColorPaletteSize)
            {
                LedColor c;
                if (parseHexColor(v, c))
                    config.colorPalette.push_back(c);
            }
        }
        else if (k == "colorLayout")
        {
            if (v == "VelocityBased")
                config.colorLayout = PianoLedConfig::LedStripColorLayout::VelocityBased;
            else if (v == "NoteBased")
                config.colorLayout = PianoLedConfig::LedStripColorLayout::NoteBased;
        }
        else if (k == "noteOffColor")
        {
            LedColor c;
            if (parseHexColor(v, c))
                config.noteOffColor = c;
        }
        else if (k == "noteOffColorBrightness")
            config.noteOffColorBrightness = v.toInt();
        else if (k == "midiChannelsToListen")
        {
            config.midiChannelsToListen.clear();
            int start = 0;
            while (start < v.length())
            {
                int comma = v.indexOf(',', start);
                String s = (comma < 0) ? v.substring(start) : v.substring(start, comma);
                s.trim();
                if (s.length())
                    config.midiChannelsToListen.push_back(s.toInt());
                if (comma < 0)
                    break;
                start = comma + 1;
            }
        }
        else if (k == "lowestKey")
            config.lowestKey = std::string(v.c_str());
    }

    // choose a default mapping if none was specified
    config.colorCurve = GradientColorMapping::Linear;

    return parsed_lines > 0;
}

void ConfigManager::printConfig(const PianoLedConfig &config)
{
    // Print strips
    for (size_t i = 0; i < config.strips.size(); ++i)
    {
        Serial.print("strip[");
        Serial.print(i);
        Serial.println("]:");
        Serial.print("  ledPin = ");
        Serial.println(config.strips[i].ledPin);
        Serial.print("  totalLeds = ");
        Serial.println(config.strips[i].totalLeds);
        Serial.print("  ledsPerMeter = ");
        Serial.println(config.strips[i].ledsPerMeter);
        Serial.print("  stripToPianoLengthScale = ");
        Serial.println(config.strips[i].stripToPianoLengthScale);
        Serial.print("  stripOrientation = ");
        switch (config.strips[i].stripOrientation)
        {
        case PianoLedStrip::StripOrientation::LeftToRight:
            Serial.println("LeftToRight");
            break;
        case PianoLedStrip::StripOrientation::RightToLeft:
            Serial.println("RightToLeft");
            break;
        case PianoLedStrip::StripOrientation::StackedLeftToRight:
            Serial.println("StackedLeftToRight");
            break;
        case PianoLedStrip::StripOrientation::StackedRightToLeft:
            Serial.println("StackedRightToLeft");
            break;
        default:
            Serial.println("Unknown");
            break;
        }
    }

    // Print colorPalette
    for (size_t i = 0; i < config.colorPalette.size(); ++i)
    {
        auto &c = config.colorPalette[i];
        char buf[8];
        sprintf(buf, "%02X%02X%02X", c.r, c.g, c.b);
        Serial.print("colorPalette[");
        Serial.print(i);
        Serial.print("] = #");
        Serial.println(buf);
    }

    // Print colorLayout
    Serial.print("colorLayout = ");
    switch (config.colorLayout)
    {
    case PianoLedConfig::LedStripColorLayout::VelocityBased:
        Serial.println("VelocityBased");
        break;
    case PianoLedConfig::LedStripColorLayout::NoteBased:
        Serial.println("NoteBased");
        break;
    default:
        Serial.println("Unknown");
        break;
    }

    // Print noteOffColor
    {
        auto &c = config.noteOffColor;
        char buf[8];
        sprintf(buf, "%02X%02X%02X", c.r, c.g, c.b);
        Serial.print("noteOffColor = #");
        Serial.println(buf);
    }

    // Print noteOffColorBrightness
    Serial.print("noteOffColorBrightness = ");
    Serial.println(config.noteOffColorBrightness);

    // Print midiChannelsToListen
    Serial.print("midiChannelsToListen = ");
    for (size_t i = 0; i < config.midiChannelsToListen.size(); ++i)
    {
        if (i)
            Serial.print(',');
        Serial.print(config.midiChannelsToListen[i]);
    }
    Serial.println();

    // Print lowestKey
    Serial.print("lowestKey = ");
    Serial.println(config.lowestKey.c_str());
}

// helper: "#RRGGBB" → LedColor
bool ConfigManager::parseHexColor(const String &v, LedColor &out)
{
    if (v.length() < 7 || v.charAt(0) != '#')
        return false;
    // use strtoul so we don't rely on sscanf with char signedness
    char buf[8];
    v.substring(1, 7).toCharArray(buf, sizeof(buf));
    char *end = nullptr;
    unsigned long rgb = strtoul(buf, &end, 16);
    if (!end || *end != '\0')
        return false;
    out.r = (rgb >> 16) & 0xFF;
    out.g = (rgb >> 8) & 0xFF;
    out.b = (rgb >> 0) & 0xFF;
    return true;
}

bool ConfigManager::beginFS()
{
    if (fsReady)
        return true;

    const uint32_t FS_SIZE = 1024 * 1024;

    if (!fs.begin(FS_SIZE))
    {
        Serial.println("LittleFS begin failed, formatting...");
        if (!fs.format(FS_SIZE))
        { // format the region once
            Serial.println("LittleFS format failed");
            return false;
        }
        if (!fs.begin(FS_SIZE))
        { // mount after format
            Serial.println("LittleFS begin after format failed");
            return false;
        }
    }

    fsReady = true;
    return true;
}
