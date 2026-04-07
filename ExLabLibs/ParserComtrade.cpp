#include "ParserComtrade.h"
#include "include.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>



ParserComtrade::ParserComtrade(const string& cfg_file_, const string& dat_file_) : cfg_file(move(cfg_file_)), dat_file(move(dat_file_))
{
    //////////////////////////////////////////////////

    ifstream cfg(cfg_file);
    string line;

    if (!cfg.is_open()) throw runtime_error("Could not open CFG file");

    getline(cfg, line);
    getline(cfg, line);

    stringstream ss_count(line);
    string tmp;
    getline(ss_count, tmp, ',');
    int total_ch = stoi(tmp);

    for (int i = 0; i < total_ch; ++i)
    {
        getline(cfg, line);
        if (line.find(",A") == string::npos && line.find(",P") == string::npos) continue;

        stringstream ss(line);
        vector<string> tokens;
        while (getline(ss, tmp, ',')) tokens.push_back(tmp);

        AnalogChannel ch;
        ch.index = stoi(tokens[0]);
        ch.name = tokens[1];
        ch.a = stod(tokens[5]);
        ch.b = stod(tokens[6]);
        analogChannels.push_back(ch);
    }

    analogData.resize(analogChannels.size());
    
    ////////////////////////////////////////////////////////////////
    
    ifstream dat(dat_file);
    if (!dat.is_open()) throw runtime_error("Could not open DAT file");

    while (getline(dat, line))
    {
        if(line.empty()) continue;
        stringstream ss(line);
        string tmp;

        getline(ss, tmp, ',');

        getline(ss, tmp, ',');
        if (!tmp.empty())
        {
            double time_sec = stod(tmp)/1000000.0;
            timeData.push_back(time_sec);
        }
        

        for (int i = 0; i < analogChannels.size(); ++i)
        {
            getline(ss, tmp, ',');
            if (!tmp.empty())
            {
                double raw = stod(tmp);
                double real_val = (raw * analogChannels[i].a) + analogChannels[i].b;
                analogData[i].push_back(real_val);
            }            
        }
        total_samples++;
    }
    
    // analog_count = parseAnalogCountFromCfg(cfg_file);
}




/*
// trim helpers
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                               [](unsigned char ch)
                               { return !std::isspace(ch); }));
}
static inline void rtrim(string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
                    [](unsigned char ch)
                    { return !std::isspace(ch); })
                .base(),
            s.end());
}
static inline void trim(string &s)
{
    ltrim(s);
    rtrim(s);
}

// split by comma OR whitespace (COMTRADE often comma-separated in cfg/dat)
static vector<string> splitCsvOrWs(const string &line)
{
    vector<string> out;
    string token;
    bool hasComma = (line.find(',') != std::string::npos);

    if (hasComma)
    {
        stringstream ss(line);
        while (getline(ss, token, ','))
        {
            trim(token);
            out.push_back(token); // ← БЕЗ if (!token.empty())
        }
    }

    else
    {
        stringstream ss(line);
        while (ss >> token)
            out.push_back(token);
    }
    return out;
}

int ParserComtrade::parseAnalogCountFromCfg(const string &cfgPath)
{
    ifstream in(cfgPath);
    if (!in)
        throw logic_error("Cannot open cfg file: " + cfgPath);

    string line;

    getline(in, line); // 1 строка
    if (!in)
        throw logic_error("CFG: missing first line");

    getline(in, line); // 2 строка
    if (!in)
        throw logic_error("CFG: missing channel count line");

    auto parts = splitCsvOrWs(line);

    int aCount = 0;

    for (const auto &p : parts)
    {
        if (!p.empty() && (p.back() == 'A' || p.back() == 'a'))
        {
            string num = p.substr(0, p.size() - 1);
            trim(num);
            aCount = stoi(num);
            break;
        }
    }

    if (aCount <= 0)
        throw logic_error("CFG: cannot parse analog channel count");

    // 🔴 ВАЖНО: читаем строки аналоговых каналов
    analogChannels.clear();

    for (int i = 0; i < aCount; ++i)
    {
        if (!getline(in, line))
            throw logic_error("CFG: missing analog channel line");

        auto cols = splitCsvOrWs(line);

        if (cols.size() < 7)
            throw logic_error("CFG: invalid analog channel line - expected at least 7 columns, got " + std::to_string(cols.size()));

        AnalogChannel ch;
        ch.index = stoi(cols[0]);
        ch.name = cols[1];
        ch.unit = cols[4];
        ch.a = stod(cols[5]);
        ch.b = stod(cols[6]);

        analogChannels.push_back(ch);
    }

    return aCount;
}

vector<vector<double>> ParserComtrade::scan_comtrade()
{
    if (analog_count <= 0)
        throw logic_error("Analog count is not initialized");

    ifstream in(dat_file);
    if (!in)
        throw logic_error("Cannot open dat file: " + dat_file);

    vector<vector<double>> channels(static_cast<size_t>(analog_count));
    analogData.clear();
    timeData.clear();

    string line;
    while (getline(in, line))
    {
        trim(line);
        if (line.empty())
            continue;

        // Используем универсальный split
        vector<string> cols = splitCsvOrWs(line);

        if (static_cast<int>(cols.size()) < 2 + analog_count)
        {
            throw logic_error("DAT: line has too few columns: " + line);
        }

        // Время в секундах
        double t = stod(cols[1]) / 1e6;
        timeData.push_back(t);

        if (analogChannels.size() != static_cast<size_t>(analog_count))
        {
            throw logic_error("CFG analog channels not initialized correctly");
        }

        // Аналоговые каналы
        for (int ch = 0; ch < analog_count; ++ch)
        {
            double raw = stod(cols[2 + ch]);

            double scaled = analogChannels[ch].a * raw + analogChannels[ch].b;

            channels[ch].push_back(scaled);
        }
    }

    if (timeData.empty())
        throw logic_error("Time data is empty");

    analogData = channels;
    return channels;
}

*/