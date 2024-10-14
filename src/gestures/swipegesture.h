#pragma once

#include "effect/globals.h"
#include "gesture.h"

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, KWin::SwipeDirection direction, qreal threshold);

    KWin::SwipeDirection direction() const { return m_direction; }

    bool thresholdReached(const QPointF &delta) const;
private:
    const KWin::SwipeDirection m_direction;
    const qreal m_threshold;
};