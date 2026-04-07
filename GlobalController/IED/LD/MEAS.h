#ifndef MEAS_H
#define MEAS_H

#include "../../../include.h"
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

    MEAS(string LDName_);
    void acceptSV(vector<double> sv);
    void calculateFourier();
    void sendFourierDataToMMXU();
    void sendLSVSDataToMMXU();
};

#endif