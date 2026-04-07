#ifndef PTOC_H
#define PTOC_H

#include "CDC/ACT.h"
#include "CDC/ACD.h"
#include "CDC/ASG.h"
#include "CDC/ING.h"
#include "CDC/WYE.h"
#include "GenLogicalNodeClass.h"
// #include "../../../../../GlobalTimeController.h"
#include "../../../../include.h"

class PTOC : public GenLogicalNodeClass
{
public:
    ACT Op;     //действие на отключение ступени МТЗ
    ACD Str;     //пуск ступени МТЗ
    ASG StrVal;     //Уставка по току
    ING OpDlTmms;     //Уставка по времени
    WYE A;     //трехфазное измерение фазных токов
    double tStr;     //время пуска защиты

    PTOC(string LogicalNodeName_ = NULL, string LogicalDeviceRef_ = NULL);
    void setStrVal(double strVal);
    void setOpDlTmms(double OpDlTmms);
    void acceptDataFromMMXU(WYE wye);
    void checkStr(double timedat);
    void checkReturn();
    void checkTimeStr(double timedat);
};

#endif