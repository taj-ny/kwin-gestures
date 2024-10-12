#pragma once

#include "effect/globals.h"
#include "gesture.h"

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::SwipeDirection direction, QPointF threshold);

    KWin::SwipeDirection direction() const { return m_direction; }

    bool thresholdReached(const QPointF &delta) const;
private:
    const KWin::SwipeDirection m_direction;
    const QPointF m_threshold;
};