#include "GlobalTimeController.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace std;

// Инициализация static-полей
double GlobalTimeController::t_0 = 0.0;
double GlobalTimeController::t   = 0.0;

void GlobalTimeController::setT0(string& dateString)
{
    // Ожидаемый формат:
    // "YYYY-MM-DD HH:MM:SS.mmmmmm"
    // Пример: "2023-01-15 14:30:25.123456"
    
    // Разбор строки формата "YYYY-MM-DD HH:MM:SS.mmmmmm"
    int year = stoi(dateString.substr(0, 4));
    int month = stoi(dateString.substr(5, 2));
    int day = stoi(dateString.substr(8, 2));
    int hour = stoi(dateString.substr(11, 2));
    int minute = stoi(dateString.substr(14, 2));
    int second = stoi(dateString.substr(17, 2));
    
    // Найти точку и получить микросекунды
    std::size_t dotPos = dateString.find('.', 19);
    long microseconds = 0;
    if (dotPos != string::npos) {
        microseconds = stol(dateString.substr(dotPos + 1));
    }
    
    // Приблизительный расчет времени в миллисекундах с 1 января 1970 года
    // Упрощенная версия, без учета всех нюансов календаря и временных зон
    
    // Количество дней с начала эпохи Unix (1 января 1970)
    // Считаем количество дней до начала года
    int daysFromYear = (year - 1970) * 365;
    
    // Добавляем дни за високосные годы
    for (int y = 1970; y < year; y++) {
        if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
            daysFromYear++;
        }
    }
    
    // Массив дней в каждом месяце (невисокосный год)
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Если год високосный, февраль имеет 29 дней
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    
    // Добавляем дни за месяцы до текущего
    for (int m = 0; m < month - 1; m++) {
        daysFromYear += daysInMonth[m];
    }
    
    // Добавляем дни месяца
    daysFromYear += day - 1; // -1, потому что день считается с 1
    
    // Общее количество секунд
    long long totalSeconds = static_cast<long long>(daysFromYear) * 24 * 60 * 60;
    totalSeconds += hour * 3600;
    totalSeconds += minute * 60;
    totalSeconds += second;
    
    // Переводим в миллисекунды и добавляем микросекунды (преобразованные в миллисекунды)
    double timeMs = static_cast<double>(totalSeconds) * 1000.0 + static_cast<double>(microseconds) / 1000.0;

    t_0 = timeMs;
    t   = 0.0;
}

void GlobalTimeController::addTimeStamp()
{
    // Python: t = t + 0.250
    // 0.250 мс
    t += 0.250;
}

double GlobalTimeController::getCurrentTime()
{
    return t;
}
