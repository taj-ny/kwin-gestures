#include "swipegesture.h"

namespace libgestures
{

bool SwipeGesture::thresholdReached(const qreal &delta) const
{
    return std::abs(delta) >= std::abs(m_threshold);
}

void SwipeGesture::setDirection(const libgestures::SwipeDirection &direction)
{
    m_direction = direction;
}

} // namespace libgestures