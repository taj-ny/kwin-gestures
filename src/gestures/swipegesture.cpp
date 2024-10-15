#include "swipegesture.h"

SwipeGesture::SwipeGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold, SwipeDirection direction)
    : Gesture(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold),
      m_direction(direction)
{
}

bool SwipeGesture::thresholdReached(const qreal &delta) const
{
    return std::abs(delta) >= std::abs(m_threshold);
}