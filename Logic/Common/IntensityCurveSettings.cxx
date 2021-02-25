#include "IntensityCurveSettings.h"

int 
IntensityCurveSettings::GetMin(int level, int window) {
    return static_cast<int>((2 * level - window) / 2.0);
}

int 
IntensityCurveSettings::GetMax(int level, int window) {
    return static_cast<int>((2 * level + window) / 2.0);
}

int 
IntensityCurveSettings::GetLevel(int min, int max) {
    return static_cast<int>((min + max) / 2.0);
}

int 
IntensityCurveSettings::GetWindow(int min, int max) {
    return max - min;
}

Vector2d
IntensityCurveSettings::GetTValue(int level, int window, Vector2d irange) {
    int min = GetMin(level, window);
    int max = GetMax(level, window);

    int t1 = (max - irange[0]) / (irange[1] - irange[0]);
    int t0 = (min - irange[0]) / (irange[1] - irange[0]);

    return Vector2d(t0, t1);
}