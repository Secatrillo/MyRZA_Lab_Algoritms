#ifndef RADR_H
#define RADR_H

#include "CDC/SPS.h"
#include "GenLogicalNodeClass.h"
#include "../../../../include.h"

class RADR : public GenLogicalNodeClass
{
public:
    SPS ChTrg;
    string ChName;
    vector<double> ChData;
    vector<double> tData;

    RADR(string LogicalNodeName_, string LogicalDeviceRef_, string ChName_);
    void catchAnalogValue(double data, double tData);
    vector<double> getAnalogValues();
    vector<double> getTimeData();
};

#endif