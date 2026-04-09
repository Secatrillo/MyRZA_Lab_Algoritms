#include "LSVS.h"

LSVS::LSVS(std::string LogicalNodeName_ , std::string LogicalDeviceRef_ ):
    GenLogicalNodeClass(LogicalNodeName_,LogicalDeviceRef_),
    currentA(std::make_unique<SAV>("Мгновенный ток А", LogicalNodeName_ , false)),
    currentB(std::make_unique<SAV>("Мгновенный ток В", LogicalNodeName_ , false)),
    currentC(std::make_unique<SAV>("Мгновенный ток С", LogicalNodeName_ , false))
    {}

void LSVS::acceptIncomingSV(std::shared_ptr<std::vector<double>> svMessage){
    currentA->setInstMag(svMessage->at(0));
    currentB->setInstMag(svMessage->at(1));
    currentC->setInstMag(svMessage->at(2));
}

// std::array<std::shared_ptr<SAV>,3> LSVS::sendSampledValues(){
//     return std::array<std::shared_ptr<SAV>,3> ({currentA,currentB,currentC});
// }