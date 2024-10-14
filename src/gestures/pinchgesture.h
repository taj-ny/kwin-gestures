#pragma once

#include "effect/globals.h"
#include "gesture.h"
#include <QRegularExpression>

class PinchGesture : public Gesture
{
public:
    PinchGesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, KWin::PinchDirection direction, qreal threshold);

    KWin::PinchDirection direction() const { return m_direction; }

    bool thresholdReached(const qreal &scale) const;
private:
    const KWin::PinchDirection m_direction;
    const qreal m_threshold;
};