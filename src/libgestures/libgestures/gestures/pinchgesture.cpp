#include "pinchgesture.h"

namespace libgestures
{

bool PinchGesture::thresholdReached(const qreal &scale) const
{
    switch (m_direction)
    {
        case PinchDirection::Contracting:
            return scale <= m_threshold;
        case PinchDirection::Expanding:
            return scale >= m_threshold;
        default:
            return scale >= m_threshold || scale <= m_threshold;
    }
}

void PinchGesture::setDirection(const PinchDirection &direction)
{
    m_direction = direction;
}

} // namespace libgestures