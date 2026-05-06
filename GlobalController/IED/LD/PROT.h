#pragma once

#include "LN/CDC/WYE.h"
#include "GenLogicalDeviceClass.h"
#include "LN/PIOC.h"
#include "LN/PSCH.h"
#include "LN/PTRC.h"
#include "LN/Fourier&RMS.h"
#include "LN/CDC/CMV.h"
#include <ParserComtrade.h>
#include <zmq.hpp>


class PROT : public GenLogicalDeviceClass
{
public:
    PIOC PIOC1;   // от PositiveSeq
    PIOC PIOC2;   // от NegativeSeq
    PSCH PSCH1;
    PTRC PTRC1;

    PROT(std::string LDName_);

    void setSettings(double posStrVal, double posStrAng, double posTimeS,
                     double negStrVal, double negStrAng, double negTimeS,
                     double kman, double iManThr,
                     std::shared_ptr<ParserComtrade> parser);

    void acceptDataFromMSQI(std::shared_ptr<CMV> positiveSeq,
                            std::shared_ptr<CMV> negativeSeq);

    void linkFourierToPSCH(Fourier& f);
    void linkPSCHToPIOC();

    void bindPSCHLocal(zmq::context_t& ctx, const std::string& selfName);
    void connectPSCHRemote(zmq::context_t& ctx, const std::string& remoteName);

    void imitateRP(int& timedat);

    std::shared_ptr<std::vector<double>> times;
};
