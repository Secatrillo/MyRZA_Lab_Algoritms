#include "LSVS.h"

LSVS::LSVS(std::string LogicalNodeName_ , std::string LogicalDeviceRef_ ):
    GenLogicalNodeClass(LogicalNodeName_,LogicalDeviceRef_),
    currentA(std::make_unique<SAV>("Мгновенный ток А", LogicalNodeName_ , false)),
    currentB(std::make_unique<SAV>("Мгновенный ток В", LogicalNodeName_ , false)),
    currentC(std::make_unique<SAV>("Мгновенный ток С", LogicalNodeName_ , false))
    {}

void LSVS::acceptIncomingSV(int& i){
    currentA->setInstMag(analogData->at(0).at(i));
    currentB->setInstMag(analogData->at(1).at(i));
    currentC->setInstMag(analogData->at(2).at(i));
}

void LSVS::setParserData(std::shared_ptr<ParserComtrade> parserData){
    analogData = std::make_unique<std::vector<std::vector<double>>>(parserData->getAnalogData());
}

// std::array<std::shared_ptr<SAV>,3> LSVS::sendSampledValues(){
//     return std::array<std::shared_ptr<SAV>,3> ({currentA,currentB,currentC});
// }