#include "TimeQuality.h"
//Класс типа данных качества времени временной метки атрибута данных
TimeQuality::TimeQuality()
{
    time_quality.push_back(LeapSecondKnown);
    time_quality.push_back(ClockFailure);
    time_quality.push_back(ClockNotSynchronized);
    time_quality.push_back(TimeAccuracy);
}