#include "RBDR.h"

RBDR::RBDR(std::string LogicalNodeName_, std::string LogicalDeviceRef_, std::string ChName_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_ + "/" + LogicalNodeName_),
      ChTrg(std::make_shared<SPS>(ChName_ + ".Trg", this->getLNRef(), false)),
      ChName(ChName_)
{
}

void RBDR::catchDiscreteSignal(bool discreteSignal, double tData) {
    ChData.push_back(discreteSignal ? 1 : 0);
    this->tData.push_back(tData);
}

std::vector<int> RBDR::getDiscreteValues() {
    return ChData;
}