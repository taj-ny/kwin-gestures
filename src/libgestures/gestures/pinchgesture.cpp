#include "pinchgesture.h"

PinchGesture::PinchGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold, PinchDirection direction)
    : Gesture(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold),
      m_direction(direction)
{
}

bool PinchGesture::thresholdReached(const qreal &scale) const
{
    switch (m_direction)
    {
        case PinchDirection::Contracting:
            return scale <= m_threshold;
        case PinchDirection::Expanding:
            return scale >= m_threshold;
        default:
            return scale >= m_threshold || scale <= m_threshold;
    }
}