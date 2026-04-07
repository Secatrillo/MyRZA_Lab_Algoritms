#include "include.h"
#include "GenDataObjectClass.h"

using namespace std;

GenDataObjectClass::GenDataObjectClass(
    string DataObjectName_, string DataObjectRef_, bool Presence_, string DataObjectType_):
    DataObjectName(DataObjectName_), DataObjectRef(DataObjectRef_), Presence(Presence_), DataObjectType(DataObjectType_)
    {}
