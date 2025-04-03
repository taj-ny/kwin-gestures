#pragma once

#include "trigger.h"

namespace libgestures
{

enum class TriggerSpeed
{
    Any,
    Slow,
    Fast
};
Q_ENUM_NS(TriggerSpeed)
Q_DECLARE_FLAGS(TriggerSpeeds, TriggerSpeed)
Q_DECLARE_OPERATORS_FOR_FLAGS(TriggerSpeeds)

class MotionTriggerUpdateEvent : public TriggerUpdateEvent
{
public:
    MotionTriggerUpdateEvent() = default;

    // Speed should be in a TriggerBeginEvent, but that's not a thing, and adding it would complicate everything.
    // Not worth it for a single property.
    const TriggerSpeed &speed() const;
    void setSpeed(const TriggerSpeed &speed);

    const QPointF &deltaMultiplied() const;
    void setDeltaMultiplied(const QPointF &delta);

private:
    TriggerSpeed m_speed = TriggerSpeed::Any;
    QPointF m_deltaMultiplied{};
};

/**
 * An input action that involves directionless motion.
 */
class MotionTrigger : public Trigger
{
public:
    /**
     * @return Whether the speed matches.
     * @see Trigger::canUpdate
     */
    bool canUpdate(const TriggerUpdateEvent *event) const override;

    bool hasSpeed() const;
    /**
     * Ignored unless set.
     * @param speed Speed the gesture must be performed at.
     */
    void setSpeed(const TriggerSpeed &speed);

private:
    TriggerSpeed m_speed = TriggerSpeed::Any;
};

}