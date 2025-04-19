#include <Arduino.h>

#include "GradientColorMapping.h"
#include "LedColor.h"
#include <sstream>
#include <iomanip>

LedColor GradientColorMapping::Map(int number, double upperBound, std::function<double(double)> func, const std::vector<LedColor>& colors) {
    double normalized = (number - 1) / (upperBound - 1);  // Normalize input to range 0-1
    double transformed = func(normalized);  // Apply custom mathematical function

    // Ensure transformed value stays within [0,1]
    transformed = std::max(0.0, std::min(1.0, transformed));

    int segmentCount = colors.size() - 1;  // Number of color transitions
    double scaled = transformed * segmentCount;  // Scale based on segment count
    int index = static_cast<int>(std::floor(scaled));  // Get the lower bound segment
    double localT = scaled - index;  // Get the local interpolation factor

    // Ensure index stays within bounds
    if (index >= segmentCount) {
        return colors.back();
    }

    // Get start and end colors for this segment
    const LedColor& start = colors[index];
    const LedColor& end = colors[index + 1];

    // Interpolate between the two colors
    int r = static_cast<int>(start.r + (end.r - start.r) * localT);
    int g = static_cast<int>(start.g + (end.g - start.g) * localT);
    int b = static_cast<int>(start.b + (end.b - start.b) * localT);
    LedColor ledColor(r, g, b);
    return ledColor;
}
