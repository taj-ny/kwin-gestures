#include "pinchgesture.h"

namespace libgestures
{

void PinchGesture::setDirection(const PinchDirection &direction)
{
    m_direction = direction;
}

qreal PinchGesture::getRealDelta(const QPointF &delta) const
{
    return m_direction == PinchDirection::In
        ? delta.x() * -1
        : delta.x();
}

}