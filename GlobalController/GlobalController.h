#ifndef GLOBALCONTROLLER_H
#define GLOBALCONTROLLER_H

#include "../ExLabLibs/ParserComtrade.h"
// #include "GlobalTimeController.h"
#include "IED/IED.h"
#include "../include.h"
#include <stdexcept>

class GlobalController
{
public:
    ParserComtrade& comtradeParser;
    vector<vector<double>> allDataMas;
    string start_datetime;
    vector<double> DatafromIED;

    // полезно для отладки
    const vector<vector<double>>& getAllData() const { return allDataMas; };
    static size_t getSamplesCountOrThrow(const std::vector<std::vector<double>>& data);
    GlobalController(ParserComtrade& parser_);
    void imitateIEDWork(IED& ied, ParserComtrade& parser, vector<double> ReceiveMMXU);
};

#endif