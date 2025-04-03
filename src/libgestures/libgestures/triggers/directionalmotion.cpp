#include "directionalmotion.h"

namespace libgestures
{

bool DirectionalMotionTrigger::canUpdate(const TriggerUpdateEvent *event) const
{
    if (!MotionTrigger::canUpdate(event)) {
        return false;
    }

    const auto *castedEvent = static_cast<const DirectionalMotionTriggerUpdateEvent *>(event);
    return m_direction & castedEvent->direction();
}

const uint32_t &DirectionalMotionTriggerUpdateEvent::direction() const
{
    return m_direction;
}

void DirectionalMotionTriggerUpdateEvent::setDirection(const uint32_t &direction)
{
    m_direction = direction;
}

}