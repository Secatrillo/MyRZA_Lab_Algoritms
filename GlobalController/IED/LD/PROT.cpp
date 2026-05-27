#include "PROT.h"

#include "LN/CDC/ACT.h"

#include <cmath>

PROT::PROT(std::string LDName_)
    : GenLogicalDeviceClass(LDName_),
      PTRC1("PTRC1", LDName_),
      RDIS_P("RDIS_P", LDName_),
      RDIS_N("RDIS_N", LDName_),
      BlkSvgCombined(std::make_shared<ACT>("BlkSvgCombined", LDName_, false)),
      PDIS1("PDIS1", LDName_),
      PDIS2("PDIS2", LDName_),
      PDIS3("PDIS3", LDName_)
{
    PDIS1.linkBlkSvg(BlkSvgCombined);
    PDIS2.linkBlkSvg(BlkSvgCombined);
    PDIS3.linkBlkSvg(BlkSvgCombined);
}

void PROT::linkDistanceProtection(std::shared_ptr<WYE> curZ)
{
    PDIS1.linkCurZ(curZ);
    PDIS2.linkCurZ(curZ);
    PDIS3.linkCurZ(curZ);
}

void PROT::setSettings(double posStrVal, double posStrAng,
                       double negStrVal, double negStrAng,
                       std::shared_ptr<ParserComtrade> parser,
                       const PdisPickupSettings& pdis1,
                       const PdisPickupSettings& pdis2,
                       const PdisPickupSettings& pdis3)
{
    times = std::make_shared<std::vector<double>>(parser->getTimeData());

    RDIS_P.setStrVal(posStrVal * 0.5);
    RDIS_P.setStrAng(posStrAng);
    RDIS_P.setOpDlTmms(0.08);

    RDIS_N.setStrVal(negStrVal * 0.5);
    RDIS_N.setStrAng(negStrAng);
    RDIS_N.setOpDlTmms(0.06);

    auto applyPdis = [](PDIS& p, const PdisPickupSettings& z) {
        p.setStrValR(z.strValR);
        p.setStrValX(z.strValX);
        p.setStrValFiRad(z.strValFiRad);
        p.setStrValOffset(z.strValOffset);
        p.setStrValHyst(z.strValHyst);
        p.setOpDlTmms(z.opDlTmms);
    };
    applyPdis(PDIS1, pdis1);
    applyPdis(PDIS2, pdis2);
    applyPdis(PDIS3, pdis3);

    PDIS1.bumpZoneRevision();
    PDIS2.bumpZoneRevision();
    PDIS3.bumpZoneRevision();
}

void PROT::acceptDataFromMSQI(std::shared_ptr<CMV> positiveSeq, std::shared_ptr<CMV> negativeSeq)
{
    RDIS_P.acceptDataFromMSQI(positiveSeq);
    RDIS_N.acceptDataFromMSQI(negativeSeq);
}

void PROT::imitateRP(int& timedat)
{
    const int idx = timedat;
    const double t = times->at(timedat);

    RDIS_P.checkStr(idx, t);
    RDIS_N.checkStr(idx, t);
    RDIS_P.checkTimeStr(idx, t);
    RDIS_N.checkTimeStr(idx, t);

    /* Blk RDIS: true = блокировка активна (нет превышения по приращению). Блокируем PDIS, пока оба
     * канала (P и N) в таком «спокойном» состоянии; превышение на любом канале снимает общую блокировку. */
    const bool blk = (RDIS_P.Blk->general->getvalue() != 0) && (RDIS_N.Blk->general->getvalue() != 0);
    BlkSvgCombined->general->setvalue(blk ? 1 : 0);

    PDIS1.step(idx, t);
    PDIS2.step(idx, t);
    PDIS3.step(idx, t);

    PTRC1.formTrip({PDIS1.Op, PDIS2.Op, PDIS3.Op});
}
