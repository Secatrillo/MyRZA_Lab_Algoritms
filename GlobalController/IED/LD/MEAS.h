#pragma once


#include "GenLogicalDeviceClass.h"
#include "LN/LSVS.h"
#include "LN/MMXU.h"
#include "LN/Fourier&RMS.h"
#include "LN/MSQI.h"
#include "ParserComtrade.h"

class MEAS : public GenLogicalDeviceClass
{
public:
    LSVS LSVS1;
    MMXU MMXU1;
    Fourier Fourier1;
    MSQI MSQI1;

    MEAS(std::string LDName_);
    void acceptSV(int& i);
    void setSettings(bool mode, int discrit, std::shared_ptr<ParserComtrade> parser);
    void calculateFourier();
    void sendFourierDataToMMXU();
    void sendLSVSDataToMMXU();
    void sendMMXUDataToMSQI();
    void calculateSequenses();
};

