#include "include.h"
#include "GenDataAttributeClass.h"

using namespace std;

GenDataAttributeClass::GenDataAttributeClass(string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_,
                          string DataAttributeType_, string helpDataType_, double value_, string DataObjectRef_):
                          DataAttributeName(DataAttributeName_), FunctionalConstraint(FunctionalConstraint_), TrgOp(TrgOp_),
                          DataAttributeType(DataAttributeType_), helpDataType(helpDataType_), value(value_), DataAttributeRef(DataObjectRef_+"$"+DataAttributeName_)
                          {
                          }
          