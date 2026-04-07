#ifndef FOURIER_H
#define FOURIER_H

#include <cmath>
#include "CDC/SAV.h"
#include "CDC/CDT/Vector.h"
#include "CDC/CDT/EnumFunctionalConstraints.h"
#include "CDC/CDT/TriggerOption.h"
#include "GenLogicalNodeClass.h"
#include "../../../../include.h"

class Fourier : public GenLogicalNodeClass
{
public:
    SAV currentA;
    SAV currentB;
    SAV currentC;
    
    // int N=80;
    vector<double> masA;
    vector<double> masB;
    vector<double> masC;
    int pointer=0;

    Vector fourierA;
    Vector fourierB;
    Vector fourierC;

    Fourier(string LogicalNodeName_, string LogicalDeviceRef_);
    void recieveSampledValues(double currentA_, double currentB_, double currentC_);
    void unpackSampledValues();
    void calculateFourier();
};

#endif