#include "CSWI.h"

CSWI::CSWI(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_ + "/" + LogicalNodeName_),
      Pos(std::make_shared<DPC>("Pos", this->getLNRef(), false)),
      OpOpn(std::make_shared<ACT>("OpOpn", this->getLNRef(), false)),
      Tr(std::make_shared<ACT>("Tr", this->getLNRef(), false))
{
    
    Pos->stVal->setvalue(1);
}

void CSWI::receiveTrip(std::shared_ptr<ACT> TripSignal) {
    
    if (TripSignal) {
        OpOpn = TripSignal;
    }
}

void CSWI::changePos() {
    
}