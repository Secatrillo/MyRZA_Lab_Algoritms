#pragma once

#include "CDC/DPC.h"
#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"

class CSWI : public GenLogicalNodeClass
{
public:
    std::shared_ptr<DPC> Pos;
    std::shared_ptr<ACT> OpOpn;
    std::shared_ptr<ACT> Tr;   
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    CSWI(std::string LogicalNodeName_=NULL, std::string LogicalDeviceRef_=NULL);
    void receiveTrip(std::shared_ptr<ACT>  TripSignal);
    void changePos();
};

