#include "GlobalController/IED/IED.h"
#include "include.h"

int main()
{
    IED ied1("MTZ1");
    std::string cfg_file1 = "End_line/K3_1.cfg";
    std::string dat_file1 = "End_line/K3_1.dat";
    std::string output_file1 = "output1.txt";
    std::string plotscript_file1 = "plotscript1.gp";
    std::string png_file1 = "png1.png";
    try {
        ParserComtrade parser1(cfg_file1, dat_file1);
        const auto &times = parser1.getTimeData();
        const auto &currentsA = parser1.getChannelData(0);
        const auto &currentsB = parser1.getChannelData(1);
        const auto &currentsC = parser1.getChannelData(2);
        int count1 = 0;

        ied1.setSettings(10000, 0.1, 2000, 0.6, true, 4000);
        // Прогоняем все выборки через IED, чтобы DR заполнил данные
        for (size_t i = 0; i < times.size(); ++i)
        {
            std::shared_ptr<std::vector<double>> SVMessage = std::make_shared<std::vector<double>>();
            SVMessage->push_back(currentsA[i]);
            SVMessage->push_back(currentsB[i]);
            SVMessage->push_back(currentsC[i]);
            count1++;
            ied1.modelIEDWork(SVMessage, times[i]);
        }

        ied1.watchOscill(parser1, output_file1, plotscript_file1, png_file1);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка преобразования: " << e.what() << std::endl;
        
    }    
    


    


return 0;
}