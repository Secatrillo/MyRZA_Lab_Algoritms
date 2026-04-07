#ifndef GLOBALTIMECONTROLLER_H
#define GLOBALTIMECONTROLLER_H

#include "include.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

class GlobalTimeController
{
public:
    // время в миллисекундах
    static double t_0;
    static double t;

    // Определение времени начала записи COMTRADE
    static void setT0(string& dateString);

    // Имитация увеличения времени
    static void addTimeStamp();

    static double getCurrentTime();
};

#endif