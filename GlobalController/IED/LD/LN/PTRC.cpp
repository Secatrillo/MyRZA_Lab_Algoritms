#include "PTRC.h"

PTRC::PTRC(std::string LogicalNodeName_, std::string LogicalDeviceRef_):
    GenLogicalNodeClass(LogicalNodeName_,LogicalDeviceRef_),
    Tr(std::make_shared<ACT>("Срабатывание МТЗ",this->getLNRef(),false))
    {}

void PTRC::formTrip(std::vector<std::shared_ptr<ACT>> Trip_vec){
    for(int i = 0; i < Trip_vec.size(); i++){
        if(Trip_vec.at(i)->general->getvalue()){
            Tr = Trip_vec.at(i);
            break;
        }
    }
}