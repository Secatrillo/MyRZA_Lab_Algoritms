#include "GlobalController.h"

/*
GlobalController::GlobalController(string cfg_file, string dat_file, string start_datetime_) : comtradeParser(cfg_file, dat_file),
                                                                                               start_datetime(start_datetime_)
{
    allDataMas = comtradeParser.scan_comtrade();
    GlobalTimeController::setT0(start_datetime);
}
*/
size_t GlobalController::getSamplesCountOrThrow(const vector<vector<double>> &data)
{
    if (data.size() < 3)
        throw std::runtime_error("COMTRADE data must contain at least 3 analog channels (A,B,C).");

    const size_t n = data[0].size();
    if (n == 0)
        throw std::runtime_error("COMTRADE data is empty.");

    if (data[1].size() != n || data[2].size() != n)
        throw std::runtime_error("COMTRADE channels A,B,C must have equal sample count.");

    return n;
}

GlobalController::GlobalController(ParserComtrade &parser_) : comtradeParser(parser_)
{
}

void GlobalController::imitateIEDWork(IED &ied, ParserComtrade& parser, vector<double> ReceiveMMXU)
{
    const auto &times = parser.getTimeData();
    const auto &currentsА = parser.getChannelData(0);
    const auto &currentsB = parser.getChannelData(1);
    const auto &currentsC = parser.getChannelData(2);

    
    
    // Прогоняем все выборки через IED, чтобы DR заполнил данные
    for (size_t i = 0; i < times.size(); ++i)
    {
        vector<double> SVMessage;
        vector<double> MMXUMeas;
        SVMessage.push_back(currentsА[i]);
        SVMessage.push_back(currentsB[i]);
        SVMessage.push_back(currentsC[i]);
        // ied.modelIEDWork(SVMessage, times[i]);
    }    

}
