#pragma once

#include "motiontriggerhandler.h"

namespace libgestures
{

enum class PinchType
{
    Unknown,
    Pinch,
    Rotate
};

class MultiTouchMotionTriggerHandler : public MotionTriggerHandler
{
public:
    bool pinchUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta);

protected:
    void reset() override;

private:
    qreal m_previousPinchScale = 1;
    PinchType m_pinchType = PinchType::Unknown;
    qreal m_accumulatedRotateDelta = 0;
};

}