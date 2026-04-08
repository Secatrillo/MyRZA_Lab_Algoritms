#pragma once

#include "GenLogicalNodeClass.h"
#include "CDC/SEQ.h"
#include "CDC/CMV.h"
#include <cmath>
#include <complex>


class MSQI : public GenLogicalNodeClass
{
public:
    std::shared_ptr<SEQ> A;
    std::shared_ptr<CMV> PositiveSeq;
    std::shared_ptr<CMV> NegativeSeq;
    std::shared_ptr<CMV> ZeroSeq;

    MSQI(std::string LogicalNodeName_ = NULL, std::string LogicalDeviceRef_ = NULL);
    void Calculate();

};

