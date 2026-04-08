#pragma once

#include "GenDataAttributeClass.h"
#include "TriggerOption.h"
#include "EnumFunctionalConstraints.h"
#include "ValidityEnum.h"
#include "SourceEnum.h"
#include <string>

class Quality: public GenDataAttributeClass{
    public:

        Quality(std::string DataAttributeName_, EnumFunctionalConstraints FunctionalConstraint_, TriggerOption TrgOp_, std::string DataObjectRef_);
        ValidityEnum getValidity();
        bool         getOverflow();
        bool         getOutOfRange();
        bool         getBadReference();
        bool         getOscillatory();
        bool         getFailure();
        bool         getOldData();
        bool         getInconsistent();
        bool         getInaccurate();
        SourceEnum   getSource();
        bool         getTest();
        bool         getOperatorBlocked();


        void setValidity(ValidityEnum val);
        void setOverflow(bool flag);
        void setOutOfRange(bool flag);
        void setBadReference(bool flag);
        void setOscillatory(bool flag);
        void setFailure(bool flag);
        void setOldData(bool flag);
        void setInconsistent(bool flag);
        void setInaccurate(bool flag);
        void setSource(SourceEnum src);
        void setTest(bool flag);
        void setOperatorBlocked(bool flag);



    private:
        int16_t bitMask ;
        ValidityEnum validity;
        // bool overflow;
        // bool outOfRange;
        // bool badReference;
        // bool oscillatory;
        // bool failure;
        // bool oldData;
        // bool inconsistent;
        // bool inaccurate;
        SourceEnum source;
        // bool test;
        // bool operatorBlocked;
};
