#include "MMXU.h"

MMXU::MMXU(std::string LogicalNodeName_ , std::string LogicalDeviceRef_ ):
    GenLogicalNodeClass(LogicalNodeName_,LogicalDeviceRef_),
    A(std::make_shared<WYE>("Триплет действующих значений тока", LogicalNodeName_ , false)),
    currentA(std::make_unique<SAV>("Мгновенный ток А", LogicalNodeName_ , false)),
    currentB(std::make_unique<SAV>("Мгновенный ток В", LogicalNodeName_ , false)),
    currentC(std::make_unique<SAV>("Мгновенный ток С", LogicalNodeName_ , false))
    {}