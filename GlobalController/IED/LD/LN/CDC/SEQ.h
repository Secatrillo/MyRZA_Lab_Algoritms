#pragma once

#include "GenCommonDataClass.h"
#include "GenDataObjectClass.h"
#include "CMV.h"
#include <string>
#include <array>

class SEQ : public GenCommonDataClass, public GenDataObjectClass
{
public:
    std::shared_ptr<CMV> phsA;
    std::shared_ptr<CMV> phsB;
    std::shared_ptr<CMV> phsC;
    
    SEQ(std::string DataObjectName_, std::string LNRef_, bool Presence_);
    // std::array<std::shared_ptr<CMV>,3> sendData();
};

