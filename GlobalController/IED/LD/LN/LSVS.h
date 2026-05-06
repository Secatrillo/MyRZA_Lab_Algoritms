#pragma once

#include "GenLogicalNodeClass.h"
#include "CDC/SAV.h"
#include <string>
#include <array>
#include <vector>
#include <ParserComtrade.h>

class LSVS : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SAV> currentA;
    std::shared_ptr<SAV> currentB;
    std::shared_ptr<SAV> currentC;    

    LSVS(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
    void acceptIncomingSV(int& i);
    void setParserData(std::shared_ptr<ParserComtrade> parserData);

    std::unique_ptr<std::vector<std::vector<double>>> analogData; 
    // std::array<std::shared_ptr<SAV>,3> sendSampledValues();
};
