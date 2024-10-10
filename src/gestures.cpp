#include "gestures.h"

namespace KWin
{

Gesture::Gesture(int minimumFingerCount, int maximumFingerCount, bool triggerAfterReachingThreshold)
    : m_minimumFingerCount(minimumFingerCount), m_maximumFingerCount(maximumFingerCount), m_triggerAfterReachingThreshold(triggerAfterReachingThreshold)
{
}

int Gesture::minimumFingerCount() const
{
    return m_minimumFingerCount;
}

int Gesture::maximumFingerCount() const
{
    return m_maximumFingerCount;
}

bool Gesture::triggerAfterReachingThreshold() const
{
    return m_triggerAfterReachingThreshold;
}

SwipeGesture::SwipeGesture(SwipeDirection direction, int minimumFingerCount, int maximumFingerCount, bool triggerAfterReachingThreshold, const QPointF &threshold)
    : Gesture(minimumFingerCount, maximumFingerCount, triggerAfterReachingThreshold), m_direction(direction), m_threshold(threshold)
{
}

SwipeDirection SwipeGesture::direction() const
{
    return m_direction;
}

bool SwipeGesture::thresholdReached(const QPointF &delta) const
{
    switch (m_direction)
    {
        case SwipeDirection::Up:
        case SwipeDirection::Down:
            return std::abs(delta.y()) >= std::abs(m_threshold.y());
        case SwipeDirection::Left:
        case SwipeDirection::Right:
            return std::abs(delta.x()) >= std::abs(m_threshold.x());
        default:
            Q_UNREACHABLE();
    }
}

PinchGesture::PinchGesture(PinchDirection direction, int minimumFingerCount, int maximumFingerCount, bool triggerAfterReachingThreshold, qreal threshold)
    : Gesture(minimumFingerCount, maximumFingerCount, triggerAfterReachingThreshold), m_direction(direction), m_threshold(threshold)
{
}

PinchDirection PinchGesture::direction() const
{
    return m_direction;
}

bool PinchGesture::thresholdReached(const qreal &scale) const
{
    return m_direction == PinchDirection::Expanding
        ? scale >= m_threshold
        : scale <= m_threshold;
}

}