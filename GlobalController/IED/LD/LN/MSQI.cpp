#include "MSQI.h"

using namespace std::complex_literals;

MSQI::MSQI(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_),
      A(std::make_shared<SEQ>("Триплет последовательностей тока", this->getLNRef(), false)),
      PositiveSeq(std::make_shared<CMV>("Положительная последовательность", false, this->getLNRef())),
      NegativeSeq(std::make_shared<CMV>("Отрицательная последовательность", false, this->getLNRef())),
      ZeroSeq(std::make_shared<CMV>("Нулевая последовательность", false, this->getLNRef()))
{}

void MSQI::Calculate(){
    std::complex PS = A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) 
                    + A->phsB->cVal->getMag()*pow(M_E,1i * A->phsB->cVal->getAng()) * pow(M_E,1i* (2.0/3)*M_PI)
                    + A->phsC->cVal->getMag()*pow(M_E,1i * A->phsC->cVal->getAng()) * pow(M_E,1i*-(2.0/3)*M_PI);
    PositiveSeq->cVal->setMag(abs(PS/3.0));
    PositiveSeq->cVal->setAng(arg(PS/3.0));
    std::complex NS = A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) 
                    + A->phsB->cVal->getMag()*pow(M_E,1i * A->phsB->cVal->getAng()) * pow(M_E,1i*-(2.0/3)*M_PI)
                    + A->phsC->cVal->getMag()*pow(M_E,1i * A->phsC->cVal->getAng()) * pow(M_E,1i* (2.0/3)*M_PI);
    NegativeSeq->cVal->setMag(abs(NS/3.0));
    NegativeSeq->cVal->setAng(arg(NS/3.0));
    std::complex ZS = A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) 
                    + A->phsB->cVal->getMag()*pow(M_E,1i * A->phsB->cVal->getAng()) 
                    + A->phsC->cVal->getMag()*pow(M_E,1i * A->phsC->cVal->getAng());
    ZeroSeq->cVal->setMag(abs(ZS/3.0));
    ZeroSeq->cVal->setAng(arg(ZS/3.0));
}