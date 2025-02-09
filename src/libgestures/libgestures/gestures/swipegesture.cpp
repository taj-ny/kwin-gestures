#include "swipegesture.h"

namespace libgestures
{

void SwipeGesture::setDirection(const libgestures::SwipeDirection &direction)
{
    m_direction = direction;
}

qreal SwipeGesture::getRealDelta(const QPointF &delta) const
{
    if (m_direction == SwipeDirection::Left) {
        return delta.x() * -1;
    }
    if (m_direction == SwipeDirection::Up) {
        return delta.y() * -1;
    }

    return m_direction == SwipeDirection::Right || m_direction == SwipeDirection::LeftRight
        ? delta.x()
        : delta.y();
}

}