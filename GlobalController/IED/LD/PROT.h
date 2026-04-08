#pragma once

#include "LN/CDC/WYE.h"
#include "GenLogicalDeviceClass.h"
#include "LN/PTOC.h"
#include "LN/PTRC.h"
#include "LN/CDC/CMV.h"


class PROT : public GenLogicalDeviceClass
{
public:
    PTOC PTOC1;
    PTOC PTOC2;
    PTRC PTRC1;
    PROT(std::string LDName_);
    void setSettings(double tmOpPTOC1, double currentOpPTOC1, double tmOpPTOC2, double currentOpPTOC2);
    void imitateRP(double timedat);
    void acceptDataFromMSQI(std::shared_ptr<CMV> cmv);
};

