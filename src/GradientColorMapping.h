#ifndef GRADIENT_COLOR_MAPPING_H
#define GRADIENT_COLOR_MAPPING_H

#include <vector>
#include <functional>
#include <cmath>
#include <algorithm>
#include "LedColor.h"

class GradientColorMapping {
public:
    static double Linear(double x) { return x; }
    static double Quadratic(double x) { return x * x; }
    static double SquareRoot(double x) { return std::sqrt(x); }
    static double Logarithmic(double x) { return std::log10(x) + 1; }
    static double Cubic(double x) { return x * x * x; }
    static double Exponential(double x) { return std::pow(2, x) - 1; }
    static double HardTransition(double x, double y) { return (x < y) ? 0 : 1; }

    static LedColor Map(int number, double upperBound, std::function<double(double)> func, const std::vector<LedColor>& colors);
};

#endif
