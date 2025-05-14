#ifndef NEO_PIXEL_COLOR_H
#define NEO_PIXEL_COLOR_H

#include <string>
#include <sstream>
#include <iomanip>
#include "LedColor.h"

struct NeoPixelColor {
    int ledNumber;
    int segmentNumber;
    std::string hexColor;
    LedColor ledColor;
    int brightness;

    NeoPixelColor(int ledNumber, int segmentNumber, const LedColor& ledColor, int brightness = 128)
        : ledNumber(ledNumber), segmentNumber(segmentNumber), ledColor(ledColor), brightness(brightness) {
          std::stringstream ss;
          ss << std::setw(2) << std::setfill('0') << std::hex << ledColor.r
            << std::setw(2) << std::setfill('0') << std::hex << ledColor.g
            << std::setw(2) << std::setfill('0') << std::hex << ledColor.b;
          hexColor = ss.str();  // Convert to HEX without '#'
        }

    // Equality comparison
    bool operator==(const NeoPixelColor& other) const {
        return ledNumber == other.ledNumber && segmentNumber == other.segmentNumber;
    }
};

// Custom hash function for NeoPixelColor
namespace std {
    template <>
    struct hash<NeoPixelColor> {
        std::size_t operator()(const NeoPixelColor& color) const {
            return (std::hash<int>()(color.ledNumber) << 1) ^ std::hash<int>()(color.segmentNumber);
        }
    };
}

#endif // NEO_PIXEL_COLOR_H
