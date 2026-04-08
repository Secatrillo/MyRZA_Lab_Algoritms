#include "include.h"
#include "GenDataAttributeClass.h"


GenDataAttributeClass::GenDataAttributeClass(std::string DataAttributeName_, 
                                             EnumFunctionalConstraints FunctionalConstraint_, 
                                             TriggerOption TrgOp_,
                                             std::string DataAttributeType_, 
                                             std::string helpDataType_, 
                                             double value_, 
                                             std::string DataObjectRef_):
    DataAttributeName(DataAttributeName_), 
    FunctionalConstraint(FunctionalConstraint_), 
    TrgOp(TrgOp_),
    DataAttributeType(DataAttributeType_), 
    helpDataType(helpDataType_), 
    value(value_), 
    DataAttributeRef(DataObjectRef_+"$"+DataAttributeName_){}


std::string               GenDataAttributeClass::getDataAttributeName(){return DataAttributeName;};
EnumFunctionalConstraints GenDataAttributeClass::getFunctionalConstraint(){return FunctionalConstraint;};
TriggerOption             GenDataAttributeClass::getTrgOp(){return TrgOp;};
std::string               GenDataAttributeClass::getDataAttributeType(){return DataAttributeType;};
std::string               GenDataAttributeClass::gethelpDataType(){return helpDataType;};
double                    GenDataAttributeClass::getvalue(){return value;};
std::string               GenDataAttributeClass::getDataAttributeRef(){return DataAttributeRef;};

void                      GenDataAttributeClass::setDataAttributeName(std::string DAName){DataAttributeName = DAName;};
void                      GenDataAttributeClass::setFunctionalConstraint(EFC FuncConstr){FunctionalConstraint = FuncConstr;};
void                      GenDataAttributeClass::setTrgOp(TriggerOption trigOption){TrgOp = trigOption;};
void                      GenDataAttributeClass::setDataAttributeType(std::string DAType){DataAttributeType = DAType;};
void                      GenDataAttributeClass::sethelpDataType(std::string HDAType){ helpDataType = HDAType;};
void                      GenDataAttributeClass::setvalue(double data){value = data;};
void                      GenDataAttributeClass::setDataAttributeRef(std::string DARef){DataAttributeRef = DARef;};
                          
          