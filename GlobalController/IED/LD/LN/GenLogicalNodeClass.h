#ifndef GENLOGICALNODECLASS_H
#define GENLOGICALNODECLASS_H

#include "../../../../include.h"

class GenLogicalNodeClass
{ //General Logical Node Class

public:
    string LogicalNodeName=NULL;
    string LNRef=NULL;
    GenLogicalNodeClass(string LogicalNodeName__, string LNRef_);
};

#endif