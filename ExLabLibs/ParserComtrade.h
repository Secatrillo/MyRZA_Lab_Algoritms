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
};

class ParserComtrade
{
public:
    ParserComtrade(const std::string& cfg_file, const std::string& dat_file);
    ParserComtrade(const std::pair<std::string, std::string>& files);

    const std::vector<double>& getChannelData(int idx) const;
    const std::vector<std::vector<double>>& getAnalogData() const;
    size_t getChannelCount() const;
    int getSamplesCount() const;
    const std::vector<double>& getTimeData() const;
    int getTimeDataSize();

private:
    std::string cfg_file;
    std::string dat_file;
    int total_samples;
    int analogCount;
    int discreteCount;
    double sampleRate;
    double timeMultiplier;
    std::vector<double> timeData;
    std::vector<std::vector<double>> analogData;
    std::vector<AnalogChannel> analogChannels;

    void readBinaryData();
};