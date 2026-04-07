#ifndef CSWI_H
#define CSWI_H

#include "CDC/DPC.h"
#include "CDC/ACT.h"
#include "GenLogicalNodeClass.h"
#include "../../../../include.h"

class CSWI : public GenLogicalNodeClass
{
public:
    DPC Pos;
    ACT OpOpn;
    ACT Tr;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
    CSWI(string LogicalNodeName_=NULL, string LogicalDeviceRef_=NULL);
    void receiveTrip(ACT TripSignal);
    void changePos();
};

#endif