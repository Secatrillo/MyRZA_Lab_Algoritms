#include "GenDataObjectClass.h"


GenDataObjectClass::GenDataObjectClass(
    std::string DataObjectName_, std::string DataObjectRef_, bool Presence_, std::string DataObjectType_):
    DataObjectName(DataObjectName_), DataObjectRef(DataObjectRef_), Presence(Presence_), DataObjectType(DataObjectType_)
    {}


std::string GenDataObjectClass::getDataObjectName() {
    return DataObjectName;
}

std::string GenDataObjectClass::getDataObjectRef() {
    return DataObjectRef;
}

bool GenDataObjectClass::getPresence() {
    return Presence;
}

std::string GenDataObjectClass::getDataObjectType() {
    return DataObjectType;
}

void GenDataObjectClass::setDataObjectName(std::string name) {
    DataObjectName = name;
}

void GenDataObjectClass::setDataObjectRef(std::string ref) {
    DataObjectRef = ref;
}

void GenDataObjectClass::setPresence(bool presence) {
    Presence = presence;
}

void GenDataObjectClass::setDataObjectType(std::string type) {
    DataObjectType = type;
}