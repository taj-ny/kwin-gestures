#include "rotate.h"

namespace libgestures
{
bool RotateGesture::satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const
{
    if (!Gesture::satisfiesUpdateConditions(speed)) {
        return false;
    }

    return m_direction == RotateDirection::Any || m_direction == static_cast<RotateDirection>(direction);
}

void RotateGesture::setDirection(const RotateDirection &direction)
{
    m_direction = direction;
}

GestureType RotateGesture::type() const
{
    return GestureType::Rotate;
}

}