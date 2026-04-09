#include "ParserComtrade.h"
#include "include.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>
#include <cstring>
#include <iostream>

using namespace std;

namespace
{
string trimCopy(string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

vector<string> splitCsv(const string &line)
{
    vector<string> out;
    string token;
    stringstream ss(line);
    while (getline(ss, token, ','))
    {
        out.push_back(trimCopy(token));
    }
    return out;
}
} // namespace

static int32_t bytesToInt32(const unsigned char* bytes) {
    return (int32_t)(bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24));
}

static int16_t bytesToInt16(const unsigned char* bytes) {
    return (int16_t)(bytes[0] | (bytes[1] << 8));
}

ParserComtrade::ParserComtrade(const std::string& cfg_file_, const std::string& dat_file_)
    : cfg_file(cfg_file_), dat_file(dat_file_), total_samples(0), analogCount(0), discreteCount(0), sampleRate(0), timeMultiplier(1.0)
{
    ifstream cfg(cfg_file);
    if (!cfg.is_open())
        throw runtime_error("Could not open CFG file: " + cfg_file);

    string line;
    // 1-я строка: станция, запись, год (пропускаем)
    if (!getline(cfg, line))
        throw runtime_error("Invalid CFG: missing first line");
    
    // 2-я строка: количество каналов, например "4,3A,1D"
    if (!getline(cfg, line))
        throw runtime_error("Invalid CFG: missing channel count line");
    
    auto chanTokens = splitCsv(line);
    if (chanTokens.size() < 3)
        throw runtime_error("Invalid CFG: channel count line must have 3 fields");
    
    string analogStr = chanTokens[1]; // "3A"
    string discreteStr = chanTokens[2]; // "1D"
    if (analogStr.empty() || discreteStr.empty())
        throw runtime_error("Invalid CFG: empty channel type");
    if (analogStr.back() != 'A' || discreteStr.back() != 'D')
        throw runtime_error("Invalid CFG: channel type specifier must be A or D");
    
    analogStr.pop_back(); // удаляем 'A'
    discreteStr.pop_back(); // удаляем 'D'
    
    analogCount = stoi(analogStr);
    discreteCount = stoi(discreteStr);
    
    if (analogCount < 3)
        throw runtime_error("CFG contains less than 3 analog channels");

    // Чтение аналоговых каналов
    analogChannels.clear();
    for (int i = 0; i < analogCount; ++i) {
        if (!getline(cfg, line))
            throw runtime_error("CFG: missing analog channel line");
        auto tokens = splitCsv(line);
        if (tokens.size() < 7)
            throw runtime_error("CFG: invalid analog channel line");
        AnalogChannel ch;
        ch.index = stoi(tokens[0]);
        ch.name = tokens[1];
        ch.unit = tokens[4];
        ch.a = stod(tokens[5]);
        ch.b = stod(tokens[6]);
        analogChannels.push_back(ch);
    }

    // Пропускаем дискретные каналы
    for (int i = 0; i < discreteCount; ++i) {
        if (!getline(cfg, line))
            throw runtime_error("CFG: missing discrete channel line");
    }

    // Строка частоты (например "50")
    if (!getline(cfg, line))
        throw runtime_error("CFG: missing sample rate line");
    sampleRate = stod(line);
    
    // Строка множителя времени (обычно "1")
    if (!getline(cfg, line))
        throw runtime_error("CFG: missing time multiplier line");
    // не используем, но читаем
    double timeMul = stod(line);
    
    // Строка "4000,60001" – коэффициенты времени (пропускаем)
    if (!getline(cfg, line))
        throw runtime_error("CFG: missing time factors line");
    
    // Строка даты и времени начала записи
    if (!getline(cfg, line))
        throw runtime_error("CFG: missing start date/time line");
    // Строка даты и времени окончания записи
    if (!getline(cfg, line))
        throw runtime_error("CFG: missing end date/time line");
    
    // Строка формата данных: "BINARY" или "ASCII"
    if (!getline(cfg, line))
        throw runtime_error("CFG: missing data format line");
    string format = line;
    if (format != "BINARY")
        throw runtime_error("Only BINARY format is supported in this parser");

    // Чтение бинарного DAT-файла
    readBinaryData();
}

void ParserComtrade::readBinaryData()
{
    std::ifstream dat(dat_file, std::ios::binary);
    if (!dat.is_open())
        throw runtime_error("Could not open DAT file: " + dat_file);

    const int SAMPLE_NUM_BYTES = 4;   // номер образца (int32)
    const int TIME_BYTES = 4;         // время в микросекундах (int32)
    const int ANALOG_BYTES_PER_CH = 2; // int16
    const int DISCRETE_BYTES = 2;     // упакованные дискретные сигналы (2 байта)

    const int expectedSize = SAMPLE_NUM_BYTES + TIME_BYTES + analogCount * ANALOG_BYTES_PER_CH + DISCRETE_BYTES;

    unsigned char* buffer = new unsigned char[expectedSize];
    analogData.assign(analogCount, std::vector<double>());
    timeData.clear();
    total_samples = 0;

    while (dat.read(reinterpret_cast<char*>(buffer), expectedSize)) {
        // Номер образца (не используем)
        // int32_t sampleNum = bytesToInt32(buffer);
        // Время в микросекундах
        int32_t timeMicro = bytesToInt32(buffer + 4);
        double timeSec = timeMicro / 1000000.0;
        timeData.push_back(timeSec);

        // Аналоговые каналы
        for (int i = 0; i < analogCount; ++i) {
            int16_t raw = bytesToInt16(buffer + 4 + 4 + i * 2);
            double realVal = raw * analogChannels[i].a + analogChannels[i].b;
            analogData[i].push_back(realVal);
        }
        total_samples++;
    }

    delete[] buffer;

    if (total_samples == 0)
        throw runtime_error("DAT contains no valid samples");

    // Отладочный вывод
    cout << "DEBUG Parser: total_samples = " << total_samples << endl;
    cout << "DEBUG Parser: timeData.size() = " << timeData.size() << endl;
    for (size_t i = 0; i < analogData.size(); ++i)
        cout << "DEBUG Parser: analogData[" << i << "].size() = " << analogData[i].size() << endl;
}

const std::vector<double>& ParserComtrade::getChannelData(int idx) const {
    return analogData.at(static_cast<size_t>(idx));
}

size_t ParserComtrade::getChannelCount() const {
    return analogData.size();
}

int ParserComtrade::getSamplesCount() const {
    return total_samples;
}

const std::vector<double>& ParserComtrade::getTimeData() const {
    return timeData;
}