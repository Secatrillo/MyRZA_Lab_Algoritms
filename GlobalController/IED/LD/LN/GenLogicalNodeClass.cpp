#include "GenLogicalNodeClass.h"

GenLogicalNodeClass::GenLogicalNodeClass(std::string LogicalNodeName__, std::string LNRef_):
LogicalNodeName(LogicalNodeName__), LNRef(LNRef_)
{
}


std::string GenLogicalNodeClass::getLogicalNodeName() {
    return LogicalNodeName;
}

std::string GenLogicalNodeClass::getLNRef() {
    return LNRef;
}


void GenLogicalNodeClass::setLogicalNodeName(std::string name) {
    LogicalNodeName = name;
}

void GenLogicalNodeClass::setLNRef(std::string ref) {
    LNRef = ref;
}