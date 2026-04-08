#include "XCBR.h"

XCBR::XCBR(std::string LogicalNodeName_, std::string LogicalDeviceRef_)
    : GenLogicalNodeClass(LogicalNodeName_, LogicalDeviceRef_ + "/" + LogicalNodeName_),
      Loc(std::make_shared<SPS>("Loc", this->getLNRef(), false)),
      OpCnt(std::make_shared<INS>("OpCnt", this->getLNRef(), false)),
      Pos(std::make_shared<DPC>("Pos", this->getLNRef(), false)),
      BlckOpn(std::make_shared<SPC>("BlckOpn", this->getLNRef(), false)),
      BlckCls(std::make_shared<SPC>("BlckCls", this->getLNRef(), false))
{

    Pos->stVal->setvalue(1);  // 1 = closed
    OpCnt->stVal->setvalue(0);
}

void XCBR::actOnOpen(std::shared_ptr<ACT> Op) {
    if (Op && Op->general->getvalue() && !BlckOpn->stVal->getvalue()) {
        
        Pos->stVal->setvalue(0);
        
        int cnt = OpCnt->stVal->getvalue();
        OpCnt->stVal->setvalue(cnt + 1);
    }
}