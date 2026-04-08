#pragma once

#include <string>
#include <vector>
#include "../include.h"

struct AnalogChannel
{
    int index;
    std::string name;
    std::string unit;

    double a;
    double b;

    AnalogChannel(int index, std::string name, std::string unit, double a, double b): 
        index(index),
        name(name),
        unit(unit),
        a(a),
        b(b)
        {} 
     
};
class ParserComtrade
{
public:
    ParserComtrade(const std::string& cfg_file, const std::string& dat_file);

    const std::vector<double>& getChannelData(int idx) const {return analogData[idx];}
    int getSamplesCount() const {return total_samples;}
    const std::vector<double> &getTimeData() const { return timeData; }

    // vector<vector<double>> scan_comtrade();
    // const vector<vector<double>> &getAnalogData() const { return analogData; }
    // int getAnalogCount() const { return analog_count; }
    // const vector<double> &getTime() const { return timeData; }

private:
    std::string cfg_file;
    std::string dat_file;
    int total_samples = 0;
    double sample_rate = 0;
    int analog_count = 0;
    std::vector<double> timeData;
    std::vector<std::vector<double>> analogData;
    std::vector<AnalogChannel> analogChannels;

    // int parseAnalogCountFromCfg(const string &cfgPath);
};
