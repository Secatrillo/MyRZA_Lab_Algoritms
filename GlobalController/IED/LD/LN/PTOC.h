#pragma once

#include "CDC/ACT.h"
#include "CDC/ACD.h"
#include "CDC/ASG.h"
#include "CDC/ING.h"
#include "CDC/WYE.h"
#include "GenLogicalNodeClass.h"


class PTOC : public GenLogicalNodeClass
{
public:
    std::shared_ptr<ACT> Op;     //действие на отключение ступени МТЗ
    std::shared_ptr<ACD> Str;     //пуск ступени МТЗ
    std::shared_ptr<ASG> StrVal;     //Уставка по току
    std::shared_ptr<ING> OpDlTmms;     //Уставка по времени
    std::shared_ptr<WYE> A;     //трехфазное измерение фазных токов
    double tStr;     //время пуска защиты

    PTOC(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
    void setStrVal(double strVal);
    void setOpDlTmms(double OpDlTmms);
    void acceptDataFromMMXU(WYE wye);
    void checkStr(double timedat);
    void checkReturn();
    void checkTimeStr(double timedat);
};

