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
                    + A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) * pow(M_E,1i* 120.0)
                    + A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) * pow(M_E,1i*-120.0);
    PositiveSeq->cVal->setMag(abs(PS));
    PositiveSeq->cVal->setMag(arg(PS));
    std::complex NS = A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) 
                    + A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) * pow(M_E,1i*-120.0)
                    + A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) * pow(M_E,1i* 120.0);
    NegativeSeq->cVal->setMag(abs(NS));
    NegativeSeq->cVal->setMag(arg(NS));
    std::complex ZS = A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) 
                    + A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng()) 
                    + A->phsA->cVal->getMag()*pow(M_E,1i * A->phsA->cVal->getAng());
    ZeroSeq->cVal->setMag(abs(ZS));
    ZeroSeq->cVal->setMag(arg(ZS));
}