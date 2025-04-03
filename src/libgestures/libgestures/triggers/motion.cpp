#include "motion.h"

namespace libgestures
{

bool MotionTrigger::canUpdate(const TriggerUpdateEvent *event) const
{
    const auto *castedEvent = static_cast<const MotionTriggerUpdateEvent *>(event);
    return m_speed == TriggerSpeed::Any || m_speed == castedEvent->speed();
}

bool MotionTrigger::hasSpeed() const
{
    return m_speed != TriggerSpeed::Any;
}

void MotionTrigger::setSpeed(const TriggerSpeed &speed)
{
    m_speed = speed;
}

const TriggerSpeed &MotionTriggerUpdateEvent::speed() const
{
    return m_speed;
}

void MotionTriggerUpdateEvent::setSpeed(const TriggerSpeed &speed)
{
    m_speed = speed;
}

const QPointF &MotionTriggerUpdateEvent::deltaMultiplied() const
{
    return m_deltaMultiplied;
}

void MotionTriggerUpdateEvent::setDeltaMultiplied(const QPointF &delta)
{
    m_deltaMultiplied = delta;
}

}