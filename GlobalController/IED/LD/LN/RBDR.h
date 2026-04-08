#pragma once

#include "CDC/SPS.h"
#include "GenLogicalNodeClass.h"


class RBDR : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SPS> ChTrg;

    std::vector<int> ChData;
    std::string ChName;
    std::vector<double> tData;
    
    RBDR(std::string LogicalNodeName_, std::string LogicalDeviceRef_, std::string ChName_);
    void catchDiscreteSignal(bool discreteSignal, double tData);
    std::vector<int> getDiscreteValues();
};
