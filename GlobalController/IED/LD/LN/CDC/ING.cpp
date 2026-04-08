#include "ING.h"
//integer status settings class
ING::ING(std::string DataObjectName_, std::string LNRef_, bool Presence_):
GenCommonDataClass("ING"),
GenDataObjectClass(DataObjectName_, LNRef_+"/"+DataObjectName_, Presence_, "ING"),
setVal(std::make_unique<GDAClass>("setMag", EnumFunctionalConstraints::SP, TriggerOption(true), "INT32"))
{
}