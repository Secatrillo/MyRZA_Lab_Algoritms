#pragma once

#include <cmath>
#include "CDC/SAV.h"
#include "CDC/CDT/Vector.h"
#include "CDC/CDT/EnumFunctionalConstraints.h"
#include "CDC/CDT/TriggerOption.h"
#include "GenLogicalNodeClass.h"
#include <vector>
#include <string>

class Fourier : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SAV> currentA;
    std::shared_ptr<SAV> currentB;
    std::shared_ptr<SAV> currentC;
    
    // int N=80;
    std::shared_ptr<std::vector<double>> masA;
    std::shared_ptr<std::vector<double>> masB;
    std::shared_ptr<std::vector<double>> masC;
    int pointer=0;

    std::shared_ptr<Vector> fourierA;
    std::shared_ptr<Vector> fourierB;
    std::shared_ptr<Vector> fourierC;

    Fourier(std::string LogicalNodeName_, std::string LogicalDeviceRef_);
    void recieveSampledValues(double currentA_, double currentB_, double currentC_);
    void unpackSampledValues();
    void calculateFourier();
};
