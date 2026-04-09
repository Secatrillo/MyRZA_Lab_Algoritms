#include "PTOC.h"

PTOC::PTOC(std::string LogicalNodeName_, std::string LogicalDeviceRef_ ):
    GenLogicalNodeClass(LogicalNodeName_,LogicalDeviceRef_),
    A(nullptr),
    Op(std::make_unique<ACT>("действие на отключение ступени МТЗ", LogicalNodeName_ , false)),
    Str(std::make_unique<ACD>("пуск ступени МТЗ", LogicalNodeName_ , false)),
    StrVal(std::make_unique<ASG>("Уставка по току", LogicalNodeName_ , false)),
    OpDlTmms(std::make_unique<ING>("Уставка по времени", LogicalNodeName_ , false)),
    tStr(0)
    {}


void PTOC::setStrVal(double strVal) {
    StrVal->setMag->f->setvalue(strVal);
}

void PTOC::setOpDlTmms(double opDlTmms) {
    OpDlTmms->setVal->setvalue(static_cast<int32_t>(opDlTmms*1e3));
}

void PTOC::acceptDataFromMSQI(std::shared_ptr<CMV> data) {
    A = data;   
}

// Проверка пуска защиты
void PTOC::checkStr(double timedat) {
    if (!A) return;   

    if (A->cVal->getMag() > StrVal->setMag->f->getvalue()) {
        if (!Str->general->getvalue()) {
            Str->general->setvalue(true);
            tStr = timedat;          
        }
    } else {
        if (Str->general->getvalue()) {
            Str->general->setvalue(false);
            Op->general->setvalue(false);   
        }
    }
}

// Проверка возврата защиты 
void PTOC::checkReturn() {
    if (!A) return;


    if (A->cVal->getMag() < StrVal->setMag->f->getvalue() && Str->general->getvalue()) {
    
        Str->general->setvalue(false);
        Op->general->setvalue(false);
    }
}

// Проверка времени срабатывания 
void PTOC::checkTimeStr(double timedat) {
    if (!Str->general->getvalue()) return;   // нет пуска — не проверяем

    double elapsed = timedat - tStr;
    int32_t delayMs = OpDlTmms->setVal->getvalue();

    if (elapsed*1e3 >= delayMs && delayMs > 0) {
        // Время выдержки истекло — действуем на отключение
        Op->general->setvalue(1);
    }
}