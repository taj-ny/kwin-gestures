#include "pinchgesture.h"

namespace libgestures
{

bool PinchGesture::satisfiesUpdateConditions(const GestureSpeed &speed, const PinchDirection &direction) const
{
    if (!Gesture::satisfiesUpdateConditions(speed)) {
        return false;
    }

    return m_direction == PinchDirection::Any || m_direction == direction;
}

void PinchGesture::setDirection(const PinchDirection &direction)
{
    m_direction = direction;
}

}