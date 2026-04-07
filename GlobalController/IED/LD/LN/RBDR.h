#ifndef RBDR_H
#define RBDR_H

#include "CDC/SPS.h"
#include "GenLogicalNodeClass.h"
#include "../../../../include.h"

class RBDR : public GenLogicalNodeClass
{
public:
    SPS ChTrg;
    vector<int> ChData;
    string ChName;
    vector<double> tData;
    RBDR(string LogicalNodeName_, string LogicalDeviceRef_, string ChName_);
    void catchDiscreteSignal(bool discreteSignal, double tData);
    vector<int> getDiscreteValues();
};

#endif