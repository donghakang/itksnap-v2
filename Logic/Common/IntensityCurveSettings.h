#ifndef INTENSITYCURVESETTINGS_H
#define INTENSITYCURVESETTINGS_H

#include "SNAPCommon.h"

/**
 * Written By Dongha Kang
 * 
 * Intensity Curve Helper.
 * Able to calculate between min/max and level window
 */
struct IntensityCurveSettings
{
    int GetMin(int level, int window);
    int GetMax(int level, int window);

    int GetLevel(int min, int max);
    int GetWindow(int min, int max);

    Vector2d GetTValue(int level, int window, Vector2d irange);

};

#endif // INTENSITYCURVESETTINGS_H
