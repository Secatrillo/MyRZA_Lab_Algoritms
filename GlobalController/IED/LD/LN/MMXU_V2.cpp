#include "MMXU_V2.h"
#include "LSVS.h"

#include <cmath>
#include <complex>

MMXU_V2::MMXU_V2(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      A(std::make_shared<WYE>("Действующие токи фаз", LogicalNodeName_, false)),
      PhV(std::make_shared<WYE>("Действующие фазные напряжения", LogicalNodeName_, false)),
      CurZ(std::make_shared<WYE>("Межфазные сопротивления", LogicalNodeName_, false)),
      currentA(nullptr),
      currentB(nullptr),
      currentC(nullptr),
      voltageAB(nullptr),
      voltageBC(nullptr),
      voltageCA(nullptr)
{}

void MMXU_V2::bindInstantFromLSVS(LSVS& currents, LSVS& lineVoltages)
{
    currentA = currents.currentA;
    currentB = currents.currentB;
    currentC = currents.currentC;
    voltageAB = lineVoltages.currentA;
    voltageBC = lineVoltages.currentB;
    voltageCA = lineVoltages.currentC;
}

void MMXU_V2::applyFourierOutputs(const std::shared_ptr<CMV>& iA,
                                  const std::shared_ptr<CMV>& iB,
                                  const std::shared_ptr<CMV>& iC,
                                  const std::shared_ptr<CMV>& uA,
                                  const std::shared_ptr<CMV>& uB,
                                  const std::shared_ptr<CMV>& uC)
{
    A->phsA = iA;
    A->phsB = iB;
    A->phsC = iC;
    PhV->phsA = uA;
    PhV->phsB = uB;
    PhV->phsC = uC;
}

void MMXU_V2::recalculateImpedance()
{
    if (!A || !PhV || !CurZ || !A->phsA || !A->phsB || !A->phsC || !PhV->phsA || !PhV->phsB || !PhV->phsC)
        return;

    const auto toC = [](const std::shared_ptr<CMV>& c) {
        return std::polar(c->cVal->getMag(), c->cVal->getAng());
    };

    const std::complex<double> Ia = toC(A->phsA);
    const std::complex<double> Ib = toC(A->phsB);
    const std::complex<double> Ic = toC(A->phsC);
    /* PhV: фазные напряжения Ua, Ub, Uc после Фурье (не межфазные). */
    const std::complex<double> Ua = toC(PhV->phsA);
    const std::complex<double> Ub = toC(PhV->phsB);
    const std::complex<double> Uc = toC(PhV->phsC);

    auto setZ = [](const std::shared_ptr<CMV>& zNode, const std::complex<double>& Unum, const std::complex<double>& Idiv) {
        if (std::abs(Idiv) < MMXU_V2::kDivEps) {
            zNode->cVal->setMag(0.0);
            zNode->cVal->setAng(0.0);
            return;
        }
        const std::complex<double> z = Unum * 1e3 / Idiv;
        zNode->cVal->setMag(std::abs(z));
        zNode->cVal->setAng(std::arg(z));
    };

    setZ(CurZ->phsA, Ua - Ub, Ia - Ib);
    setZ(CurZ->phsB, Ub - Uc, Ib - Ic);
    setZ(CurZ->phsC, Uc - Ua, Ic - Ia);
}
