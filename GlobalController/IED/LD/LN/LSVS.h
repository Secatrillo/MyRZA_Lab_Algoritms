#ifndef LSVS_H
#define LSVS_H

#include "../../../../include.h"
#include "GenLogicalNodeClass.h"
#include "CDC/SAV.h"

class LSVS : public GenLogicalNodeClass
{
public:
    SAV currentA;
    SAV currentB;
    SAV currentC;    

    LSVS(string LogicalNodeName_ = NULL, string LogicalDeviceRef_ = NULL);
    void acceptIncomingSV(vector<double> svMessage);
    array<SAV,3> sendSampledValues();
};

#endif