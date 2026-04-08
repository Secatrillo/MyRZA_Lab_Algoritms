#include "RADR.h"

RADR::RADR(std::string LogicalNodeName_, std::string LogicalDeviceRef_, std::string ChName_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_ + "/" + LogicalNodeName_),
      ChTrg(std::make_shared<SPS>(ChName_ + ".Trg", this->getLNRef(), false)),
      ChName(ChName_)
{
}

void RADR::catchAnalogValue(double data, double tdata) {
    ChData.push_back(data);
    tData.push_back(tdata);
}

std::vector<double> RADR::getAnalogValues() {
    return ChData;
}

std::vector<double> RADR::getTimeData() {
    return tData;
}