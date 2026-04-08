#include "Quality.h"
//Класс метки качества атрибута данных
Quality::Quality(std::string DataAttributeName_,
                 EFC FunctionalConstraint_, 
                 TriggerOption TrgOp_, 
                 std::string DataObjectRef_):
    GenDataAttributeClass(DataAttributeName_, 
                          FunctionalConstraint_,
                          TrgOp_, 
                          "Quality", 
                          "", 
                          0.0, 
    DataObjectRef_),
     
    validity(ValidityEnum::good), 
    bitMask(0)
    // overflow(false), 
    // outOfRange(false), 
    // badReference(false), 
    // oscillatory(false), 
    // failure(false), 
    // oldData(false), 
    // inconsistent(false), 
    // inaccurate(false), 
    // source(SourceEnum::process), 
    // test(false),
    // operatorBlocked(false)
{
}

ValidityEnum Quality::getValidity(){ return validity; }
bool         Quality::getOverflow(){ return bitMask & 0b1000000000; }
bool         Quality::getOutOfRange(){ return bitMask & 0b0100000000; }
bool         Quality::getBadReference(){ return bitMask & 0b0010000000; }
bool         Quality::getOscillatory(){ return bitMask & 0b0001000000; }
bool         Quality::getFailure(){ return bitMask & 0b0000100000; }
bool         Quality::getOldData(){ return bitMask & 0b0000010000; }
bool         Quality::getInconsistent(){ return bitMask & 0b0000001000; }
bool         Quality::getInaccurate(){ return bitMask & 0b0000000100; }
SourceEnum   Quality::getSource(){ return source; }
bool         Quality::getTest(){ return bitMask & 0b0000000010; }
bool         Quality::getOperatorBlocked(){ return bitMask & 0b0000000001; }

// Сеттеры
void Quality::setValidity(ValidityEnum val){ validity = val; }
void Quality::setOverflow(bool flag){ bitMask += flag*0b1000000000; }
void Quality::setOutOfRange(bool flag){ bitMask += flag*0b0100000000; }
void Quality::setBadReference(bool flag){ bitMask += flag*0b0010000000; }
void Quality::setOscillatory(bool flag){ bitMask += flag*0b0001000000; }
void Quality::setFailure(bool flag){ bitMask += flag*0b0000100000; }
void Quality::setOldData(bool flag){ bitMask += flag*0b0000010000; }
void Quality::setInconsistent(bool flag){ bitMask += flag*0b0000001000; }
void Quality::setInaccurate(bool flag){ bitMask += flag*0b0000000100; }
void Quality::setSource(SourceEnum src){ source = src; }
void Quality::setTest(bool flag){ bitMask += flag*0b0000000010; }
void Quality::setOperatorBlocked(bool flag){ bitMask += flag*0b0000000001; }