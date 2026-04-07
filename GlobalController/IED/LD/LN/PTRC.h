#ifndef PTRC_H
#define PTRC_H

#include "../../../../include.h"
#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"

class PTRC : public GenLogicalNodeClass
{
public:
    ACT Tr;
    PTRC(string LogicalNodeName_ = NULL, string LogicalDeviceRef_ = NULL);
    void formTrip(const vector<ACT*>& opList);
};

#endif