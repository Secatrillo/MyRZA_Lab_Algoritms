#include "PDIS.h"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdint>

PDIS::PDIS(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      StrValR(std::make_shared<ASG>("StrValR полуось вдоль φ", this->getLNRef(), false)),
      StrValX(std::make_shared<ASG>("StrValX полуось ⊥ φ", this->getLNRef(), false)),
      StrValFi(std::make_shared<ASG>("Угол макс. чувствительности", this->getLNRef(), false)),
      StrValOffset(std::make_shared<ASG>("Смещение центра вдоль φ", this->getLNRef(), false)),
      StrValHyst(std::make_shared<ASG>("Множитель зоны возврата", this->getLNRef(), false)),
      OpDlTmms(std::make_shared<ING>("Выдержка времени Op", this->getLNRef(), false)),
      Op(std::make_shared<ACT>("Срабатывание дистанционной защиты", this->getLNRef(), false)),
      Str(std::make_shared<ACD>("Пуск дистанционной защиты", this->getLNRef(), false)),
      BlkSvg(nullptr),
      CurZ(nullptr)
{
    StrValHyst->setMag->f->setvalue(1.05);
}

void PDIS::setOpDlTmms(double seconds)
{
    OpDlTmms->setVal->setvalue(static_cast<int32_t>(seconds * 1e3));
}

void PDIS::setStrValR(double ohmMajor)
{
    StrValR->setMag->f->setvalue(ohmMajor);
}

void PDIS::setStrValX(double ohmMinor)
{
    StrValX->setMag->f->setvalue(ohmMinor);
}

void PDIS::setStrValFiRad(double fiRad)
{
    StrValFi->setMag->f->setvalue(fiRad);
}

void PDIS::setStrValOffset(double offsetOhmAlongFi)
{
    StrValOffset->setMag->f->setvalue(offsetOhmAlongFi);
}

void PDIS::setStrValHyst(double scale)
{
    StrValHyst->setMag->f->setvalue(scale);
}

std::pair<double, double> PDIS::centerRX() const
{
    const double fi = StrValFi->setMag->f->getvalue();
    const double off = StrValOffset->setMag->f->getvalue();
    return {off * std::cos(fi), off * std::sin(fi)};
}

bool PDIS::insideEllipse(double r, double x, double a, double b) const
{
    if (!CurZ)
        return false;
    const auto [cx, cy] = centerRX();
    const double fi = StrValFi->setMag->f->getvalue();
    const double dr = r - cx;
    const double dx = x - cy;
    const double u = dr * std::cos(fi) + dx * std::sin(fi);
    const double v = -dr * std::sin(fi) + dx * std::cos(fi);
    const double aa = std::max(a, 1e-15);
    const double bb = std::max(b, 1e-15);
    return (u * u) / (aa * aa) + (v * v) / (bb * bb) <= 1.0 + 1e-9;
}

void PDIS::appendEllipseOutline(double a, double b, std::vector<std::pair<double, double>>& out, int segments) const
{
    const auto [cx, cy] = centerRX();
    const double fi = StrValFi->setMag->f->getvalue();
    const double c = std::cos(fi);
    const double s = std::sin(fi);
    for (int k = 0; k <= segments; ++k) {
        const double t = (2.0 * M_PI * k) / segments;
        const double u = a * std::cos(t);
        const double v = b * std::sin(t);
        const double r = cx + u * c - v * s;
        const double x = cy + u * s + v * c;
        out.emplace_back(r, x);
    }
}

void PDIS::exportPickupPolygon(std::vector<std::pair<double, double>>& out, int segments) const
{
    out.clear();
    appendEllipseOutline(StrValR->setMag->f->getvalue(), StrValX->setMag->f->getvalue(), out, segments);
}

void PDIS::exportReturnPolygon(std::vector<std::pair<double, double>>& out, int segments) const
{
    out.clear();
    const double h = StrValHyst->setMag->f->getvalue();
    appendEllipseOutline(StrValR->setMag->f->getvalue() * h, StrValX->setMag->f->getvalue() * h, out, segments);
}

static std::pair<double, double> cmvToRX(const std::shared_ptr<CMV>& c)
{
    if (!c)
        return {0.0, 0.0};
    const double mag = c->cVal->getMag();
    const double ang = c->cVal->getAng();
    const std::complex<double> z = std::polar(mag, ang);
    return {z.real(), z.imag()};
}

void PDIS::step(int sampleIndex, double timedat)
{
    (void)sampleIndex;
    if (!CurZ || !CurZ->phsA || !CurZ->phsB || !CurZ->phsC) {
        Str->general->setvalue(false);
        Op->general->setvalue(false);
        opTimerActive_ = false;
        return;
    }

    const double aP = StrValR->setMag->f->getvalue();
    const double bP = StrValX->setMag->f->getvalue();
    const double h = StrValHyst->setMag->f->getvalue();
    const double aR = aP * h;
    const double bR = bP * h;

    const auto zab = cmvToRX(CurZ->phsA);
    const auto zbc = cmvToRX(CurZ->phsB);
    const auto zca = cmvToRX(CurZ->phsC);

    const bool anyPick = insideEllipse(zab.first, zab.second, aP, bP) || insideEllipse(zbc.first, zbc.second, aP, bP)
        || insideEllipse(zca.first, zca.second, aP, bP);
    const bool allOutRet = !insideEllipse(zab.first, zab.second, aR, bR) && !insideEllipse(zbc.first, zbc.second, aR, bR)
        && !insideEllipse(zca.first, zca.second, aR, bR);

    bool strNow = Str->general->getvalue();
    if (anyPick)
        strNow = true;
    else if (allOutRet)
        strNow = false;
    Str->general->setvalue(strNow);

    const bool blk = BlkSvg && BlkSvg->general->getvalue();
    const int32_t delayMs = OpDlTmms->setVal->getvalue();

    if (!strNow || blk) {
        opTimerActive_ = false;
        Op->general->setvalue(false);
        return;
    }

    if (!opTimerActive_) {
        opTimerActive_ = true;
        tOpAnchor_ = timedat;
    }

    const double elapsedMs = (timedat - tOpAnchor_) * 1e3;
    if (delayMs <= 0 || elapsedMs >= static_cast<double>(delayMs)) {
        Op->general->setvalue(true);
    } else {
        Op->general->setvalue(false);
    }
}
