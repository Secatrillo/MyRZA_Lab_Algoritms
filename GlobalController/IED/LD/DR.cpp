#include "DR.h"
#include "stdexcept"
#include <cstdio> // popen, pclose, FILE*
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
// Логическое устройство регистратора аварийных событий
// RADR1-RADR3 - Логические узлы регистрации входных аналоговых каналов
// RADR4-RADR6 - Логические узлы регистрации измеренных значений аналоговых данных
// RBDR1-RBDR11 - Логические узлы регистрации дискретных сигналов внутри терминалов (см. названия сигналов
DR::DR(std::string LDName_) : GenLogicalDeviceClass(LDName_),
                         RDRE1("RDRE1", LDName_),
                         RADR1("RADR1", LDName_, "IaLine"),
                         RADR2("RADR2", LDName_, "IbLine"),
                         RADR3("RADR3", LDName_, "IcLine"),
                         RADR4("RADR4", LDName_, "IaFourier"),
                         RADR5("RADR5", LDName_, "IbFourier"),
                         RADR6("RADR6", LDName_, "IcFourier"),
                         RADR7("RADR7", LDName_, "PositiveSeq"),
                         RADR8("RADR8", LDName_, "NegativeSeq"),
                         RADR9("RADR9", LDName_, "ZeroSeq"),
                         RBDR1("RBDR1", LDName_, "Пуск ТО общ."),
                         RBDR2("RBDR2", LDName_, "Пуск ТО ф.А"),
                         RBDR3("RBDR3", LDName_, "Пуск ТО ф.В"),
                         RBDR4("RBDR4", LDName_, "Пуск ТО ф.С"),
                         RBDR5("RBDR5", LDName_, "Откл. от ТО"),
                         RBDR6("RBDR6", LDName_, "Пуск II ступени общ."),
                         RBDR7("RBDR7", LDName_, "Пуск II ступени ф.А"),
                         RBDR8("RBDR8", LDName_, "Пуск II ступени ф.В"),
                         RBDR9("RBDR9", LDName_, "Пуск II ступени ф.С"),
                         RBDR10("RBDR10", LDName_, "Откл. от II ступени"),
                         RBDR11("RBDR11", LDName_, "Откл. выкл."),
                         RBDR12("RBDR12", LDName_, "Положение выключателя"),
                         RADR_otherA("RADR_otherA", LDName_, "RADR_otherA"),
                         RADR_otherB("RADR_otherB", LDName_, "RADR_otherB"),
                         RADR_otherC("RADR_otherC", LDName_, "RADR_otherC")
{
    RDRE1.RcdChNum = 20;
    RDRE1.RcdAChNum = 9;
    RDRE1.RcdDChNum = 11;
    RADRch = {&RADR1, &RADR2, &RADR3, &RADR4, &RADR5, &RADR6, &RADR7, &RADR8, &RADR9};
    RBDRch = {&RBDR1, &RBDR2, &RBDR3, &RBDR4, &RBDR5, &RBDR6,
              &RBDR7, &RBDR8, &RBDR9, &RBDR10, &RBDR11};
}

void DR::registerData(std::shared_ptr<std::vector<double>> SampledValues, std::vector<double> MMXUData, std::vector<double> MSQIData, double svTime, bool PTOC1StrGen, bool PTOC1StrPhA, bool PTOC1StrPhB, bool PTOC1StrPhC, bool PTOC1OpGen, bool PTOC2StrGen, bool PTOC2StrPhA, bool PTOC2StrPhB, bool PTOC2StrPhC, bool PTOC2OpGen, bool PTRCTrip, bool BreakerClosed)
{ // Метод регистрации данных
    RADR1.catchAnalogValue(SampledValues->at(0), svTime);
    RADR2.catchAnalogValue(SampledValues->at(1), svTime);
    RADR3.catchAnalogValue(SampledValues->at(2), svTime);
    RADR4.catchAnalogValue(MMXUData[0], svTime);
    RADR5.catchAnalogValue(MMXUData[1], svTime);
    RADR6.catchAnalogValue(MMXUData[2], svTime);
    RADR7.catchAnalogValue(MSQIData[0], svTime);
    RADR8.catchAnalogValue(MSQIData[1], svTime);
    RADR9.catchAnalogValue(MSQIData[2], svTime);

    RBDR1.catchDiscreteSignal(PTOC1StrGen, svTime);
    RBDR2.catchDiscreteSignal(PTOC1StrPhA, svTime);
    RBDR3.catchDiscreteSignal(PTOC1StrPhB, svTime);
    RBDR4.catchDiscreteSignal(PTOC1StrPhC, svTime);
    RBDR5.catchDiscreteSignal(PTOC1OpGen, svTime);

    RBDR6.catchDiscreteSignal(PTOC2StrGen, svTime);
    RBDR7.catchDiscreteSignal(PTOC2StrPhA, svTime);
    RBDR8.catchDiscreteSignal(PTOC2StrPhB, svTime);
    RBDR9.catchDiscreteSignal(PTOC2StrPhC, svTime);
    RBDR10.catchDiscreteSignal(PTOC2OpGen, svTime);

    RBDR11.catchDiscreteSignal(PTRCTrip, svTime);

    RBDR12.catchDiscreteSignal(BreakerClosed, svTime);
}

void DR::registerOtherData(std::vector<double> MMXUData_, double svTime)
{
    RADR_otherA.catchAnalogValue(MMXUData_[0], svTime);
    RADR_otherB.catchAnalogValue(MMXUData_[1], svTime);
    RADR_otherC.catchAnalogValue(MMXUData_[2], svTime);
}

// Пишем 2 колонки (t, y) в .dat
static void write_xy_dat(const std::string &path,
                         const std::vector<double> &x,
                         const std::vector<double> &y)
{
    if (x.size() != y.size())
        throw std::logic_error("write_xy_dat: x and y sizes differ");

    std::ofstream out(path);
    if (!out)
        throw std::logic_error("write_xy_dat: cannot open file: " + path);

    for (size_t i = 0; i < x.size(); ++i)
        out << x[i] << " " << y[i] << "\n";
}

// Перегрузка для int->double (для дискретных)
static void write_xy_dat(const std::string &path,
                         const std::vector<double> &x,
                         const std::vector<int> &y)
{
    if (x.size() != y.size())
        throw std::logic_error("write_xy_dat: x and y sizes differ");

    std::ofstream out(path);
    if (!out)
        throw std::logic_error("write_xy_dat: cannot open file: " + path);

    for (size_t i = 0; i < x.size(); ++i)
        out << x[i] << " " << static_cast<double>(y[i]) << "\n";
}

void DR::exportForGnuplot(const ParserComtrade &parser, const std::string &filename)
{
    std::ofstream out(filename);
    if (!out.is_open())
        return;

    const auto &time = parser.getTimeData();
    const auto &phA = parser.getChannelData(0);
    const auto &phB = parser.getChannelData(1);
    const auto &phC = parser.getChannelData(2);

    // Заголовок (необязательно, gnuplot его проигнорирует, если перед ним #)
    out << "# Time\tPhaseA\tPhaseB\tPhaseC" << std::endl;

    for (size_t i = 0; i < time.size(); i++)
    {
        out << std::fixed << std::setprecision(6)
            << time[i] << "\t"
            << RADR1.ChData[i] << "\t"
            << RADR2.ChData[i] << "\t"
            << RADR3.ChData[i] << "\t"
            << RADR4.ChData[i] << "\t"
            << RADR5.ChData[i] << "\t"
            << RADR6.ChData[i] << "\t"
            << RADR7.ChData[i] << "\t"
            << RADR8.ChData[i] << "\t"
            << RADR9.ChData[i] << "\t"
            << RBDR1.ChData[i] << "\t"
            << RBDR6.ChData[i] << "\t"
            << RBDR2.ChData[i] << "\t"
            << RBDR3.ChData[i] << "\t"
            << RBDR4.ChData[i] << "\t"
            << RBDR7.ChData[i] << "\t"
            << RBDR8.ChData[i] << "\t"
            << RBDR9.ChData[i] << "\t"
            << RBDR5.ChData[i] << "\t"
            << RBDR10.ChData[i] << "\t"
            << RBDR11.ChData[i] << "\t"
            << RBDR12.ChData[i] << "\n"
            
            // << RADR_otherA.ChData[i] << "\t"
            // << RADR_otherB.ChData[i] << "\t"
            // << RADR_otherC.ChData[i] << "\n"
            ;
    }
    out.close();
    std::cout << "Data for gnuplot save in " << filename << std::endl;
}

void DR::showPlot(std::string dataFile_, std::string plotscript_file_, std::string pngFile_)
{
    std::ofstream gp(plotscript_file_);
    gp << "set terminal pngcairo size 4000, 3600 font 'Verdana,12'\n";
    gp << "set output '" << pngFile_ << "'\n";
    gp << "set multiplot layout 5,2\n";   // 5 строк, 2 столбца = 10 графиков

    // Настройки сетки и отступов
    gp << "set key spacing 1.5\n";
    gp << "set grid xtics ytics mxtics mytics lc rgb '#E0E0E0'\n";
    gp << "set xtics 0.5\n";
    gp << "set lmargin 10\n";

    // 1. Мгновенные значения токов
    gp << "set ytics 1000\n";
    gp << "plot '" << dataFile_ << "' using 1:2 with lines lc rgb 'yellow' title 'Phase A (inst)',"
       << " '" << dataFile_ << "' using 1:3 with lines lc rgb 'green' title 'Phase B (inst)',"
       << " '" << dataFile_ << "' using 1:4 with lines lc rgb 'red' title 'Phase C (inst)'\n";

    // 2. Действующие значения (Фурье)
    gp << "set ytics 1000\n";
    gp << "plot '" << dataFile_ << "' using 1:5 with lines lc rgb 'yellow' title 'Fourier A',"
       << " '" << dataFile_ << "' using 1:6 with lines lc rgb 'green' title 'Fourier B',"
       << " '" << dataFile_ << "' using 1:7 with lines lc rgb 'red' title 'Fourier C'\n";

    gp << "set ytics 100\n";
    gp << "plot '" << dataFile_ << "' using 1:8 with lines lw 2 lc rgb 'red' title 'Positive Seq',"
    << " '" << dataFile_ << "' using 1:9 with lines lw 2 lc rgb 'orange' title 'Negative Seq',"
    << " '" << dataFile_ << "' using 1:10 with lines lw 2 lc rgb 'blue' title 'Zero Seq'\n";

    // 6. PTOC1 (ТЗНП 1 ступень) – общий пуск, пофазные пуски, срабатывание
    gp << "set ytics 0.5\n";
    gp << "plot '" << dataFile_ << "' using 1:11 with steps lc rgb 'red' title 'PTOC1 Start (gen)',"
       << " '" << dataFile_ << "' using 1:13 with steps lc rgb 'yellow' title 'PTOC1 Start A',"
       << " '" << dataFile_ << "' using 1:14 with steps lc rgb 'green' title 'PTOC1 Start B',"
       << " '" << dataFile_ << "' using 1:15 with steps lc rgb 'orange' title 'PTOC1 Start C',"
       << " '" << dataFile_ << "' using 1:19 with steps lc rgb 'blue' title 'PTOC1 Operate'\n";

    // 7. PTOC2 (ТЗНП 2 ступень) – общий пуск, пофазные пуски, срабатывание
    gp << "set ytics 0.5\n";
    gp << "plot '" << dataFile_ << "' using 1:12 with steps lc rgb 'red' title 'PTOC2 Start (gen)',"
       << " '" << dataFile_ << "' using 1:16 with steps lc rgb 'yellow' title 'PTOC2 Start A',"
       << " '" << dataFile_ << "' using 1:17 with steps lc rgb 'green' title 'PTOC2 Start B',"
       << " '" << dataFile_ << "' using 1:18 with steps lc rgb 'orange' title 'PTOC2 Start C',"
       << " '" << dataFile_ << "' using 1:20 with steps lc rgb 'blue' title 'PTOC2 Operate'\n";

    // 8. Общий Trip от PTRC
    gp << "set ytics 0.5\n";
    gp << "plot '" << dataFile_ << "' using 1:21 with steps lc rgb 'red' title 'PTRC Trip'\n";

    // 9. Положение выключателя (колонка 22)
    gp << "set ytics 0.5\n";
    gp << "plot '" << dataFile_ << "' using 1:22 with steps lc rgb 'green' title 'Breaker Position (1=closed,0=open)'\n";

    gp << "unset multiplot\n";
    gp.close();

    std::string gnuplotCmd = "gnuplot " + plotscript_file_;
    system(gnuplotCmd.c_str());
    std::string pngCmd = "start " + pngFile_;  // для Windows
    system(pngCmd.c_str());
}