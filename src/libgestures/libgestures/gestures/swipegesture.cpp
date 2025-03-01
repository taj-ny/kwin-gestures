#include "swipegesture.h"

namespace libgestures
{

bool SwipeGesture::satisfiesUpdateConditions(const GestureSpeed &speed, const SwipeDirection &direction) const
{
    if (!Gesture::satisfiesUpdateConditions(speed)) {
        return false;
    }

    return m_direction == SwipeDirection::Any || m_direction == direction
        || (m_direction == SwipeDirection::LeftRight && (direction == SwipeDirection::Left || direction == SwipeDirection::Right))
        || (m_direction == SwipeDirection::UpDown && (direction == SwipeDirection::Up || direction == SwipeDirection::Down));
}

void SwipeGesture::setDirection(const libgestures::SwipeDirection &direction)
{
    m_direction = direction;
}

}