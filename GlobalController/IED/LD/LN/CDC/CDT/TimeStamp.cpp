#include "TimeStamp.h"
//Класс типа метки времени атрибута данных
TimeStamp::TimeStamp(std::string DataAttributeName_,
                     EFC FunctionalConstraint_, 
                     TriggerOption TrgOp_, 
                     std::string DataObjectRef_):
    GenDataAttributeClass(DataAttributeName_,
                          FunctionalConstraint_, 
                          TrgOp_, 
                          "TimeStamp", 
                          "", 
                          0.0, 
                          DataObjectRef_),
    SecondSinceEpoch(0), 
    FractionOfSecond(0),
    TimeQuality_()
{}


int TimeStamp::getSecondSinceEpoch(){
    return SecondSinceEpoch;
}

int TimeStamp::getFractionOfSecond() {
    return FractionOfSecond;
}

TimeQuality TimeStamp::getTimeQuality(){
    return TimeQuality_;
}


void TimeStamp::setSecondSinceEpoch(int sec){
    SecondSinceEpoch = sec;
}

void TimeStamp::setFractionOfSecond(int frac){
    FractionOfSecond = frac;
}

void TimeStamp::setTimeQuality(TimeQuality tq){
    TimeQuality_ = tq;
}
