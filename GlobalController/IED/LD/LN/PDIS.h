#pragma once

#include "CDC/ACT.h"
#include "CDC/ACD.h"
#include "CDC/ASG.h"
#include "CDC/ING.h"
#include "CDC/WYE.h"
#include "GenLogicalNodeClass.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

/**
 * Ступень дистанционной защиты: эллипс в плоскости R–X с гистерезисом по контуру на 5% (StrValHyst).
 */
class PDIS : public GenLogicalNodeClass
{
public:
    std::shared_ptr<ASG> StrValR;
    std::shared_ptr<ASG> StrValX;
    std::shared_ptr<ASG> StrValFi;
    std::shared_ptr<ASG> StrValOffset;
    std::shared_ptr<ASG> StrValHyst;
    std::shared_ptr<ING> OpDlTmms;

    std::shared_ptr<ACT> Op;
    std::shared_ptr<ACD> Str;
    /** Внешний сигнал блокировки (общий PTRC_BLK). */
    std::shared_ptr<ACT> BlkSvg;

    std::shared_ptr<WYE> CurZ;

    PDIS(std::string LogicalNodeName_, std::string LogicalDeviceRef_);

    void linkCurZ(std::shared_ptr<WYE> curZ) { CurZ = std::move(curZ); }
    void linkBlkSvg(std::shared_ptr<ACT> blk) { BlkSvg = std::move(blk); }

    void setOpDlTmms(double seconds);
    void setStrValR(double ohmMajor);
    void setStrValX(double ohmMinor);
    void setStrValFiRad(double fiRad);
    void setStrValOffset(double offsetOhmAlongFi);
    void setStrValHyst(double scale);

    void step(int sampleIndex, double timedat);

    uint64_t getZoneRevision() const { return zoneRevision_; }
    void bumpZoneRevision() { ++zoneRevision_; }

    /** Вершины контура эллипса в плоскости (R, X) для телеметрии. */
    void exportPickupPolygon(std::vector<std::pair<double, double>>& out, int segments = 72) const;
    void exportReturnPolygon(std::vector<std::pair<double, double>>& out, int segments = 72) const;

private:
    bool insideEllipse(double r, double x, double a, double b) const;
    std::pair<double, double> centerRX() const;
    void appendEllipseOutline(double a, double b, std::vector<std::pair<double, double>>& out, int segments) const;

    uint64_t zoneRevision_ = 1;
    bool opTimerActive_ = false;
    double tOpAnchor_ = 0.0;
};
