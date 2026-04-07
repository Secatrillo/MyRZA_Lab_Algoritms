#ifndef TIMEQUALITY_H
#define TIMEQUALITY_H

#include "../../../../../../include.h"

class TimeQuality
{
public:
    vector<double> time_quality;
    bool LeapSecondKnown = true;
    bool ClockFailure = false;
    bool ClockNotSynchronized = false;
    double TimeAccuracy = 0.002;

    TimeQuality();
};

#endif