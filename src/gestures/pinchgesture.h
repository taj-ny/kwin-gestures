#pragma once

#include "effect/globals.h"
#include "gesture.h"
#include <QRegularExpression>

class PinchGesture : public Gesture
{
public:
    PinchGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::PinchDirection direction, qreal threshold);

    KWin::PinchDirection direction;
    qreal threshold;

    bool thresholdReached(const qreal &scale) const;
};