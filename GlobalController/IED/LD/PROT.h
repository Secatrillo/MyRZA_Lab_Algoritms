#pragma once

#include "LN/CDC/WYE.h"
#include "GenLogicalDeviceClass.h"
#include "LN/PDIS.h"
#include <IEDSettings.h>
#include "LN/PTRC.h"
#include "LN/RDIS.h"
#include "LN/CDC/ACT.h"
#include "LN/CDC/CMV.h"
#include <ParserComtrade.h>
#include <memory>
#include <string>

class PROT : public GenLogicalDeviceClass
{
public:
    PTRC PTRC1;

    RDIS RDIS_P;
    RDIS RDIS_N;
    std::shared_ptr<ACT> BlkSvgCombined;
    PDIS PDIS1;
    PDIS PDIS2;
    PDIS PDIS3;

    PROT(std::string LDName_);

    void setSettings(double posStrVal, double posStrAng,
                     double negStrVal, double negStrAng,
                     std::shared_ptr<ParserComtrade> parser,
                     const PdisPickupSettings& pdis1,
                     const PdisPickupSettings& pdis2,
                     const PdisPickupSettings& pdis3);

    void acceptDataFromMSQI(std::shared_ptr<CMV> positiveSeq, std::shared_ptr<CMV> negativeSeq);

    void linkDistanceProtection(std::shared_ptr<WYE> curZ);

    void imitateRP(int& timedat);

    std::shared_ptr<std::vector<double>> times;
};
