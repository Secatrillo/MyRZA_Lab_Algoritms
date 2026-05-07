#include "PIOC.h"

#include <cmath>
#include <complex>
#include <cstdint>

PIOC::PIOC(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      CurrentVec(nullptr),
      PrevVec(std::make_unique<CMV>("Предыдущий вектор", false, this->getLNRef())),
      StrVal(std::make_shared<ASG>("Уставка по приращению модуля", this->getLNRef(), false)),
      StrAng(std::make_shared<ASG>("Уставка по приращению угла", this->getLNRef(), false)),
      OpDlTmms(std::make_shared<ING>("Уставка по времени", this->getLNRef(), false)),
      Op(std::make_shared<ACT>("Действие на отключение", this->getLNRef(), false)),
      Str(std::make_shared<ACD>("Пуск защиты", this->getLNRef(), false)),
      BlkDPP(nullptr)
{
}

void PIOC::setStrVal(double strVal)
{
    StrVal->setMag->f->setvalue(strVal);
}

void PIOC::setStrAng(double strAngRad)
{
    StrAng->setMag->f->setvalue(strAngRad);
}

void PIOC::setOpDlTmms(double seconds)
{
    OpDlTmms->setVal->setvalue(static_cast<int32_t>(seconds * 1e3));
}

void PIOC::acceptDataFromMSQI(std::shared_ptr<CMV> data)
{
    CurrentVec = data;
}

void PIOC::acceptBlock(std::shared_ptr<ACT> blk)
{
    BlkDPP = blk;
}



double wrapAngle(double a)
{
    while (a > M_PI)  a -= 2.0 * M_PI;
    while (a < -M_PI) a += 2.0 * M_PI;
    return a;
}



void PIOC::checkStr(int sampleIndex, double timedat)
{
    if (!CurrentVec) return;

    const double curMag = CurrentVec->cVal->getMag();
    const double curAng = CurrentVec->cVal->getAng();

    
    if (sampleIndex < kWarmupSamples) {
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        prevValid = true;
        return;
    }

    /* Первый такт после прогрева: PrevVec ещё не инициализирован реальной величиной. */
    if (!prevValid) {
        PrevVec->cVal->setMag(curMag);
        PrevVec->cVal->setAng(curAng);
        prevValid = true;
        return;
    }

    const double prevMag = PrevVec->cVal->getMag();
    const double prevAng = PrevVec->cVal->getAng();

    const std::complex<double> cur = std::polar(curMag, curAng);
    const std::complex<double> prev = std::polar(prevMag, prevAng);
    const std::complex<double> diff = cur - prev;

    const double dMag = std::abs(diff);
    const double dPhi = std::abs(wrapAngle(curAng - prevAng));

    const double thrMag = StrVal->setMag->f->getvalue();
    const double thrAng = StrAng->setMag->f->getvalue();

    /* Угловой критерий имеет смысл только при заметном приращении. */
    const bool angExceeded = (dMag > (thrMag * 0.1)) && (dPhi > thrAng);
    const bool exceeded = (dMag > thrMag) || angExceeded;

    if (exceeded) {
        if (!Str->general->getvalue()) {
            Str->general->setvalue(true);
            tStr = timedat;
        }
        /* PrevVec сохраняем — измеряем длительность относительно начала скачка. */
    } else {
        /* Гистерезис сброса пуска: при «дребезге» около уставки Str не гаснет одним тактом,
           иначе OpDlTmms не успевает набраться (типично для второго конца линии / шумной осциллограммы). */
        const bool magCalmed = (dMag < thrMag * 0.4);
        const bool angCalmed = (dPhi < thrAng * 0.75) || (dMag < thrMag * 0.05);
        if (!Str->general->getvalue()) {
            PrevVec->cVal->setMag(curMag);
            PrevVec->cVal->setAng(curAng);
        } else if (magCalmed && angCalmed) {
            PrevVec->cVal->setMag(curMag);
            PrevVec->cVal->setAng(curAng);
            Str->general->setvalue(false);
            Op->general->setvalue(false);
        }
    }
}

void PIOC::checkTimeStr(int sampleIndex, double timedat)
{
    if (sampleIndex < kWarmupSamples) return;
    if (!Str->general->getvalue()) return;

    const double elapsedMs = (timedat - tStr) * 1e3;
    const int32_t delayMs = OpDlTmms->setVal->getvalue();

    const bool blocked = BlkDPP && BlkDPP->general->getvalue();

    if (delayMs > 0 && elapsedMs >= delayMs && !blocked) {
        Op->general->setvalue(true);
    }
}
