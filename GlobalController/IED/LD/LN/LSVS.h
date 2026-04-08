#pragma once

#include "GenLogicalNodeClass.h"
#include "CDC/SAV.h"
#include <string>
#include <array>
#include <vector>

class LSVS : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SAV> currentA;
    std::shared_ptr<SAV> currentB;
    std::shared_ptr<SAV> currentC;    

    LSVS(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
    void acceptIncomingSV(std::vector<double> svMessage);
    std::array<SAV,3> sendSampledValues();
};
