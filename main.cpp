#include "IED.h"
#include "GlobalController.h"
#include "include.h"

int main()
{
    IED ied1("MTZ1");
    string cfg_file1 = "Test2/BRK1.cfg";
    string dat_file1 = "Test2/BRK1.dat";
    string output_file1 = "output1.txt";
    string plotscript_file1 = "plotscript1.gp";
    string png_file1 = "png1.png";
    ParserComtrade parser1(cfg_file1, dat_file1);
    GlobalController global_controller1(parser1);

    const auto &times = parser1.getTimeData();
    const auto &currentsА = parser1.getChannelData(0);
    const auto &currentsB = parser1.getChannelData(1);
    const auto &currentsC = parser1.getChannelData(2);
    int count1 = 0;

    ied1.setProtSettings(10000, 0.1, 2000, 0.6);
    // Прогоняем все выборки через IED, чтобы DR заполнил данные
    for (size_t i = 0; i < times.size(); ++i)
    {
        vector<double> SVMessage;
        SVMessage.push_back(currentsА[i]);
        SVMessage.push_back(currentsB[i]);
        SVMessage.push_back(currentsC[i]);
        count1++;
        ied1.modelIEDWork(SVMessage, times[i]);
    }

    ied1.watchOscill(parser1, output_file1, plotscript_file1, png_file1);


return 0;
}