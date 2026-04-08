#pragma once

#include <cmath>
#include "CDC/SAV.h"
#include "CDC/CMV.h"
#include "CDC/SPS.h"
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

    std::shared_ptr<SPS> mode;
    int N;
    double freq = 50;
    
    // int N=80;
    std::shared_ptr<std::vector<double>> masA;
    std::shared_ptr<std::vector<double>> masB;
    std::shared_ptr<std::vector<double>> masC;
    int pointer=0;

    std::shared_ptr<CMV> fourierA;
    std::shared_ptr<CMV> fourierB;
    std::shared_ptr<CMV> fourierC;

    Fourier(std::string LogicalNodeName_, std::string LogicalDeviceRef_);
    void recieveSampledValues( const double& currentA_, const double& currentB_, const double& currentC_);
    void unpackSampledValues();
    void calculateFourier(std::shared_ptr<std::vector<double>>, std::shared_ptr<Vector>, double);
};
