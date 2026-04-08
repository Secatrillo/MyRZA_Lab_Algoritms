#pragma once

#include "TriggerOption.h"
#include "EnumFunctionalConstraints.h"
#include "TimeQuality.h"
#include "GenDataAttributeClass.h"
#include <string>

class TimeStamp : public GenDataAttributeClass
{

    public:

        TimeStamp(std::string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, std::string DataObjectRef_);

        int getSecondSinceEpoch();
        int getFractionOfSecond();
        TimeQuality getTimeQuality();

        void setSecondSinceEpoch(int sec);
        void setFractionOfSecond(int frac);
        void setTimeQuality(TimeQuality tq);
        
        

    private: 
        int SecondSinceEpoch;
        int FractionOfSecond;
        TimeQuality TimeQuality_;
};

