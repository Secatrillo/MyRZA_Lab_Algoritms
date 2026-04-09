#pragma once


#include "GenLogicalDeviceClass.h"
#include "LN/LSVS.h"
#include "LN/MMXU.h"
#include "LN/Fourier&RMS.h"
#include "LN/MSQI.h"

class MEAS : public GenLogicalDeviceClass
{
public:
    LSVS LSVS1;
    MMXU MMXU1;
    Fourier Fourier1;
    MSQI MSQI1;

    MEAS(std::string LDName_);
    void acceptSV(std::shared_ptr<std::vector<double>> sv);
    void setFourierMode(bool mode, int discrit);
    void calculateFourier();
    void sendFourierDataToMMXU();
    void sendLSVSDataToMMXU();
    void sendMMXUDataToMSQI();
    void calculateSequenses();
};

