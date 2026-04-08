#include "RDRE.h"

RDRE::RDRE(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_ + "/" + LogicalNodeName_),
      RcdMade(std::make_shared<SPS>("RcdMade", this->getLNRef(), false)),
      FltNum(std::make_shared<INS>("FltNum", this->getLNRef(), false))
{
    // Инициализация по умолчанию
    RcdChNum = 0;
    RcdAChNum = 0;
    RcdDChNum = 0;
}