#pragma once

#include "GenLogicalDeviceClass.h"
#include "LN/LSVS.h"
#include "LN/MMXU_V2.h"
#include "LN/Fourier&RMS.h"
#include "LN/MSQI.h"
#include "ParserComtrade.h"

class MEAS : public GenLogicalDeviceClass
{
public:
    LSVS LSVS_I;
    LSVS LSVS_U;
    MMXU_V2 mmxuV2;
    Fourier Fourier_I;
    Fourier Fourier_U;
    MSQI MSQI1;

    MEAS(std::string LDName_);
    void acceptSV(int& i);
    void setSettings(bool mode, int discrit,
                      std::shared_ptr<ParserComtrade> parserCurrent,
                      std::shared_ptr<ParserComtrade> parserVoltage);
    void calculateSequenses();

    void calculateFourier_V2();
    void sendFourierDataToMMXU_V2();
    void sendMMXU_V2_DataToMSQI();
};
