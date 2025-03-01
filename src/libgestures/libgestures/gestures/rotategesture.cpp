#include "rotategesture.h"

namespace libgestures
{

bool RotateGesture::satisfiesUpdateConditions(const GestureSpeed &speed, const RotateDirection &direction) const
{
    if (!Gesture::satisfiesUpdateConditions(speed)) {
        return false;
    }

    return m_direction == RotateDirection::Any || m_direction == direction;
}

void RotateGesture::setDirection(const RotateDirection &direction)
{
    m_direction = direction;
}

}