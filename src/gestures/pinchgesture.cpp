#include "pinchgesture.h"

PinchGesture::PinchGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::PinchDirection direction, qreal threshold)
        : Gesture(device, triggerAfterReachingThreshold, minimumFingers, maximumFingers, windowRegex), direction(direction), threshold(threshold)
{
}

bool PinchGesture::thresholdReached(const qreal &scale) const
{
    return direction == KWin::PinchDirection::Expanding
           ? scale >= threshold
           : scale <= threshold;
}