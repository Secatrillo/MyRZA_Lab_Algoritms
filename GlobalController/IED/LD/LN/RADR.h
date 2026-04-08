#pragma once

#include "CDC/SPS.h"
#include "GenLogicalNodeClass.h"


class RADR : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SPS> ChTrg;
    
    std::string ChName;
    std::vector<double> ChData;
    std::vector<double> tData;

    RADR(std::string LogicalNodeName_, std::string LogicalDeviceRef_, std::string ChName_);
    void catchAnalogValue(double data, double tData);
    std::vector<double> getAnalogValues();
    std::vector<double> getTimeData();
};
