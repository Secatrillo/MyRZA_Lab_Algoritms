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
    enum PhaseTag { PhaseA = 0, PhaseB = 1, PhaseC = 2 };

    std::shared_ptr<SAV> currentA;
    std::shared_ptr<SAV> currentB;
    std::shared_ptr<SAV> currentC;

    std::shared_ptr<SPS> mode;
    int N = 80;
    double freq = 50;
    
    std::shared_ptr<std::vector<double>> masA;
    std::shared_ptr<std::vector<double>> masB;
    std::shared_ptr<std::vector<double>> masC;
    int pointer=0;

    std::shared_ptr<CMV> fourierA;
    std::shared_ptr<CMV> fourierB;
    std::shared_ptr<CMV> fourierC;

    /* Ortogonal components per phase (sine/cosine projections); used by PSCH. */
    double Fx[3] = {0.0, 0.0, 0.0};
    double Fy[3] = {0.0, 0.0, 0.0};

    Fourier(std::string LogicalNodeName_, std::string LogicalDeviceRef_);
    void recieveSampledValues( const double& currentA_, const double& currentB_, const double& currentC_);
    void unpackSampledValues();
    void calculateFourier(std::shared_ptr<std::vector<double>>, std::shared_ptr<Vector>, double, PhaseTag);

    double getFx(PhaseTag p) const { return Fx[p]; }
    double getFy(PhaseTag p) const { return Fy[p]; }
};
