#include "ING.h"
//integer status settings class
ING::ING(string DataObjectName_, string LNRef_, bool Presence_):
GenCommonDataClass("ING"),
GenDataObjectClass(DataObjectName_, LNRef_+"/"+DataObjectName_, Presence_, "ING"),
setVal("setMag", EnumFunctionalConstraints::SP, TriggerOption(true), "INT32")
{
}