#ifndef PROT_H
#define PROT_H

#include "LN/CDC/WYE.h"
#include "GenLogicalDeviceClass.h"
#include "LN/PTOC.h"
#include "LN/PTRC.h"
#include "../../../include.h"

class PROT : public GenLogicalDeviceClass
{
public:
    PTOC PTOC1;
    PTOC PTOC2;
    PTRC PTRC1;
    PROT(string LDName_);
    void setSettings(double tmOpPTOC1, double currentOpPTOC1, double tmOpPTOC2, double currentOpPTOC2);
    void imitateRP(double timedat);
    void acceptDataFromMMXU(WYE wye);
};

#endif