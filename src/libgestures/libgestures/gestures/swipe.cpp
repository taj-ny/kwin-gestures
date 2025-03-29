#include "swipe.h"

namespace libgestures
{

bool SwipeGesture::satisfiesUpdateConditions(const GestureSpeed &speed, const uint32_t &direction) const
{
    if (!Gesture::satisfiesUpdateConditions(speed)) {
        return false;
    }

    const auto castedDirection = static_cast<SwipeDirection>(direction);
    return m_direction == SwipeDirection::Any || m_direction == castedDirection
        || (m_direction == SwipeDirection::LeftRight && (castedDirection == SwipeDirection::Left || castedDirection == SwipeDirection::Right))
        || (m_direction == SwipeDirection::UpDown && (castedDirection == SwipeDirection::Up || castedDirection == SwipeDirection::Down));
}

void SwipeGesture::setDirection(const libgestures::SwipeDirection &direction)
{
    m_direction = direction;
}

GestureType SwipeGesture::type() const
{
    return GestureType::Swipe;
}

}