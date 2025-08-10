#ifndef NEO_PIXEL_COLOR_H
#define NEO_PIXEL_COLOR_H

#include <string>
#include <sstream>
#include <iomanip>
#include "LedColor.h"

struct NeoPixelColor {
    int stripNumber; // The strip number this LED belongs to
    int ledNumber;
    std::string hexColor;
    LedColor ledColor;
    int brightness;

    NeoPixelColor(int stripNumber, int ledNumber, const LedColor& ledColor, int brightness = 128)
        : stripNumber(stripNumber), ledNumber(ledNumber), ledColor(ledColor), brightness(brightness) {
          std::stringstream ss;
          ss << std::setw(2) << std::setfill('0') << std::hex << ledColor.r
            << std::setw(2) << std::setfill('0') << std::hex << ledColor.g
            << std::setw(2) << std::setfill('0') << std::hex << ledColor.b;
          hexColor = ss.str();  // Convert to HEX without '#'
        }

    // Equality comparison
    bool operator==(const NeoPixelColor& other) const {
        return ledNumber == other.ledNumber && stripNumber == other.stripNumber;
    }
};

// Custom hash function for NeoPixelColor
namespace std {
    template <>
    struct hash<NeoPixelColor> {
        std::size_t operator()(const NeoPixelColor& color) const {
            return (std::hash<int>()(color.ledNumber) << 1) ^ std::hash<int>()(color.stripNumber);
        }
    };
}

#endif // NEO_PIXEL_COLOR_H
