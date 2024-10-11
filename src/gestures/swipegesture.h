#pragma once

#include "effect/globals.h"
#include "gesture.h"

class SwipeGesture : public Gesture
{
public:
    SwipeGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::SwipeDirection direction, QPointF threshold);

    KWin::SwipeDirection direction;
    QPointF threshold;

    bool thresholdReached(const QPointF &delta) const;
};