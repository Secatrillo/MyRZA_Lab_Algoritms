#pragma once

#include <vector>

class TimeQuality
{
public:
    std::vector<double> time_quality;
    bool LeapSecondKnown = true;
    bool ClockFailure = false;
    bool ClockNotSynchronized = false;
    double TimeAccuracy = 0.002;

    TimeQuality();
};

