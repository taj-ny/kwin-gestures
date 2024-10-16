#pragma once

#include "gesture.h"

class HoldGesture : public Gesture
{
    Q_OBJECT
public:
    HoldGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold);
};