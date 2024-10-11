#include "swipegesture.h"

SwipeGesture::SwipeGesture(InputDevice device, bool triggerAfterReachingThreshold, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex, KWin::SwipeDirection direction, QPointF threshold)
    : Gesture(device, triggerAfterReachingThreshold, minimumFingers, maximumFingers, windowRegex), direction(direction), threshold(threshold)
{
}

bool SwipeGesture::thresholdReached(const QPointF &delta) const
{
    switch (direction)
    {
        case KWin::SwipeDirection::Up:
        case KWin::SwipeDirection::Down:
            return std::abs(delta.y()) >= std::abs(threshold.y());
        case KWin::SwipeDirection::Left:
        case KWin::SwipeDirection::Right:
            return std::abs(delta.x()) >= std::abs(threshold.x());
        default:
            Q_UNREACHABLE();
    }
}