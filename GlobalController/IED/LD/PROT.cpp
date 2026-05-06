#include "PROT.h"

PROT::PROT(std::string LDName_)
    : GenLogicalDeviceClass(LDName_),
      PIOC1("PIOC1", LDName_),
      PIOC2("PIOC2", LDName_),
      PSCH1("PSCH1", LDName_),
      PTRC1("PTRC1", LDName_)
{
}

void PROT::setSettings(double posStrVal, double posStrAng, double posTimeS,
                       double negStrVal, double negStrAng, double negTimeS,
                       double kman,
                       double iManThr,
                       std::shared_ptr<ParserComtrade> parser)
{
    PIOC1.setStrVal(posStrVal);
    PIOC1.setStrAng(posStrAng);
    PIOC1.setOpDlTmms(posTimeS);

    PIOC2.setStrVal(negStrVal);
    PIOC2.setStrAng(negStrAng);
    PIOC2.setOpDlTmms(negTimeS);

    PSCH1.setKman(kman);
    PSCH1.setIManThr(iManThr);

    times = std::make_shared<std::vector<double>>(parser->getTimeData());
}

void PROT::acceptDataFromMSQI(std::shared_ptr<CMV> positiveSeq,
                              std::shared_ptr<CMV> negativeSeq)
{
    PIOC1.acceptDataFromMSQI(positiveSeq);
    PIOC2.acceptDataFromMSQI(negativeSeq);
}

void PROT::linkFourierToPSCH(Fourier& f)
{
    PSCH1.setSource(f);
}

void PROT::linkPSCHToPIOC()
{
    PIOC1.acceptBlock(PSCH1.Blk);
    PIOC2.acceptBlock(PSCH1.Blk);
}

void PROT::bindPSCHLocal(zmq::context_t& ctx, const std::string& selfName)
{
    PSCH1.bindLocal(ctx, selfName);
}

void PROT::connectPSCHRemote(zmq::context_t& ctx, const std::string& remoteName)
{
    PSCH1.connectRemote(ctx, remoteName);
}

void PROT::imitateRP(int& timedat)
{
    const int idx = timedat;
    const double t = times->at(timedat);

    PSCH1.step();

    PIOC1.checkStr(idx, t);
    PIOC2.checkStr(idx, t);

    PIOC1.checkTimeStr(idx, t);
    PIOC2.checkTimeStr(idx, t);

    PTRC1.formTrip({ PIOC1.Op, PIOC2.Op });
}
