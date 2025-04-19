#ifndef LEDCOLOR_H
#define LEDCOLOR_H

class LedColor {
public:
    int r, g, b;

    constexpr LedColor() : r(0), g(0), b(0) {}                     // Default constructor
    constexpr LedColor(int r, int g, int b) : r(r), g(g), b(b) {} // Custom constructor

    // Static color presets (defined below)
    static const LedColor Red;
    static const LedColor Green;
    static const LedColor Blue;
    static const LedColor Yellow;
    static const LedColor White;
    static const LedColor Black;
};

// Now define the static constants (outside the class)
inline constexpr LedColor LedColor::Red    = LedColor(255, 0, 0);
inline constexpr LedColor LedColor::Green  = LedColor(0, 255, 0);
inline constexpr LedColor LedColor::Blue   = LedColor(0, 0, 255);
inline constexpr LedColor LedColor::Yellow = LedColor(255, 255, 0);
inline constexpr LedColor LedColor::White  = LedColor(255, 255, 255);
inline constexpr LedColor LedColor::Black  = LedColor(0, 0, 0);

#endif