#pragma once

#include <string>
#include <vector>
#include "../include.h"

struct AnalogChannel
{
    int index;
    string name;
    string unit;

    double a;
    double b;
};
class ParserComtrade
{
public:
    ParserComtrade(const string& cfg_file, const string& dat_file);

    const vector<double>& getChannelData(int idx) const {return analogData[idx];}
    int getSamplesCount() const {return total_samples;}
    const vector<double> &getTimeData() const { return timeData; }

    // vector<vector<double>> scan_comtrade();
    // const vector<vector<double>> &getAnalogData() const { return analogData; }
    // int getAnalogCount() const { return analog_count; }
    // const vector<double> &getTime() const { return timeData; }

private:
    string cfg_file;
    string dat_file;
    int total_samples = 0;
    double sample_rate = 0;
    int analog_count = 0;
    vector<double> timeData;
    vector<vector<double>> analogData;
    vector<AnalogChannel> analogChannels;

    // int parseAnalogCountFromCfg(const string &cfgPath);
};
