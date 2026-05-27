#pragma once

#include "GenLogicalNodeClass.h"
#include "CDC/WYE.h"
#include "CDC/SAV.h"
#include "CDC/CMV.h"
#include "LSVS.h"

/**
 * MMXU_V2: мгновенные токи LSVS_I и три фазных напряжения LSVS_U (после Фурье — Ua, Ub, Uc),
 * расчёт межфазных сопротивлений CurZ: Z_ab=(Ua-Ub)/(Ia-Ib), Z_bc=(Ub-Uc)/(Ib-Ic), Z_ca=(Uc-Ua)/(Ic-Ia).
 * phsA/B/C в PhV: Ua, Ub, Uc. В CurZ: Z_ab, Z_bc, Z_ca.
 */
class MMXU_V2 : public GenLogicalNodeClass
{
public:
    std::shared_ptr<WYE> A;
    std::shared_ptr<WYE> PhV;
    std::shared_ptr<WYE> CurZ;

    std::shared_ptr<SAV> currentA;
    std::shared_ptr<SAV> currentB;
    std::shared_ptr<SAV> currentC;
    std::shared_ptr<SAV> voltageAB;
    std::shared_ptr<SAV> voltageBC;
    std::shared_ptr<SAV> voltageCA;

    MMXU_V2(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);

    /** Привязка мгновенных SAV от двух LSVS (токи и три фазных напряжения). */
    void bindInstantFromLSVS(LSVS& currents, LSVS& lineVoltages);

    /** Запись выходов Фурье: токи Ia,Ib,Ic и фазные напряжения Ua,Ub,Uc. */
    void applyFourierOutputs(const std::shared_ptr<CMV>& iA,
                             const std::shared_ptr<CMV>& iB,
                             const std::shared_ptr<CMV>& iC,
                             const std::shared_ptr<CMV>& uA,
                             const std::shared_ptr<CMV>& uB,
                             const std::shared_ptr<CMV>& uC);

    /** Z_ab = (Ua-Ub)/(Ia-Ib), Z_bc = (Ub-Uc)/(Ib-Ic), Z_ca = (Uc-Ua)/(Ic-Ia). */
    void recalculateImpedance();

private:
    static constexpr double kDivEps = 1e-9;
};
