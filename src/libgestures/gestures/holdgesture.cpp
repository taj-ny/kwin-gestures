#include "holdgesture.h"

HoldGesture::HoldGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold)
    : Gesture(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold)
{
}