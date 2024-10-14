#include "swipegesture.h"

SwipeGesture::SwipeGesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, KWin::SwipeDirection direction, qreal threshold)
    : Gesture(device, triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly),
      m_direction(direction),
      m_threshold(threshold)
{
}

bool SwipeGesture::thresholdReached(const QPointF &delta) const
{
    switch (m_direction)
    {
        case KWin::SwipeDirection::Up:
        case KWin::SwipeDirection::Down:
            return std::abs(delta.y()) >= std::abs(m_threshold);
        case KWin::SwipeDirection::Left:
        case KWin::SwipeDirection::Right:
            return std::abs(delta.x()) >= std::abs(m_threshold);
        default:
            Q_UNREACHABLE();
    }
}