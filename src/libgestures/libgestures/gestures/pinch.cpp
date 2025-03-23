#include "pinch.h"

namespace libgestures
{
bool PinchGesture::satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const
{
    if (!Gesture::satisfiesUpdateConditions(speed)) {
        return false;
    }

    return m_direction == PinchDirection::Any || m_direction == static_cast<PinchDirection>(direction);
}

void PinchGesture::setDirection(const PinchDirection &direction)
{
    m_direction = direction;
}

GestureType PinchGesture::type() const
{
    return GestureType::Pinch;
}

}