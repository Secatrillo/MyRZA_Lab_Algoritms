#pragma once


#include "GenLogicalDeviceClass.h"
#include "LN/LSVS.h"
#include "LN/MMXU.h"
#include "LN/Fourier.h"

class MEAS : public GenLogicalDeviceClass
{
public:
    LSVS LSVS1;
    MMXU MMXU1;
    Fourier Fourier1;

    MEAS(std::string LDName_);
    void acceptSV(std::vector<double> sv);
    void calculateFourier();
    void sendFourierDataToMMXU();
    void sendLSVSDataToMMXU();
};

