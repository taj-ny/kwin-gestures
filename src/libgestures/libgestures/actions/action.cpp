#include "action.h"

namespace libgestures
{

void GestureAction::addCondition(const std::shared_ptr<const Condition> &condition)
{
    m_conditions.push_back(condition);
}

bool GestureAction::satisfiesConditions() const
{
    return m_conditions.empty() || std::find_if(m_conditions.begin(), m_conditions.end(), [](const std::shared_ptr<const Condition> &condition) {
        return condition->isSatisfied();
    }) != m_conditions.end();
}

bool GestureAction::thresholdReached() const
{
    return (m_minimumThreshold == 0 || m_absoluteAccumulatedDelta >= m_minimumThreshold)
        && (m_maximumThreshold == 0 || m_absoluteAccumulatedDelta <= m_maximumThreshold);
}

void GestureAction::tryExecute()
{
    if (!canExecute()) {
        return;
    }

    execute();
    m_executed = true;
}

const bool &GestureAction::executed() const
{
    return m_executed;
}

bool GestureAction::canExecute() const
{
    return satisfiesConditions() && thresholdReached();
}

bool GestureAction::blocksOtherActions() const
{
    return m_executed && m_blockOtherActions;
}

void GestureAction::gestureCancelled(const bool &execute)
{
    if (execute && (m_on == On::Cancel || m_on == On::EndOrCancel)) {
        tryExecute();
    }

    m_executed = false;
    reset();
}

void GestureAction::gestureEnded(const bool &execute)
{
    if (execute && (m_on == On::End || m_on == On::EndOrCancel)) {
        tryExecute();
    }

    m_executed = false;
    reset();
}

void GestureAction::gestureStarted()
{
    m_executed = false;
    if (m_on == On::Begin) {
        tryExecute();
    }

    reset();
}

void GestureAction::gestureUpdated(const qreal &delta, const QPointF &deltaPointMultiplied)
{
    m_currentDeltaPointMultiplied = deltaPointMultiplied;
    if ((m_accumulatedDelta > 0 && delta < 0) || (m_accumulatedDelta < 0 && delta > 0)) {
        // Direction changed
        m_accumulatedDelta = delta;
    } else {
        m_accumulatedDelta += delta;
        m_absoluteAccumulatedDelta += std::abs(delta);
    }

    if (m_on != On::Update) {
        return;
    }
    const auto interval = m_interval.value();
    if (interval == 0 && m_interval.matches(delta)) {
        tryExecute();
        return;
    }

    // Keep executing action until accumulated delta no longer exceeds the interval
    while (m_interval.matches(m_accumulatedDelta) && std::abs(m_accumulatedDelta / interval) >= 1) {
        tryExecute();
        m_accumulatedDelta -= interval;
    }
}

const On &GestureAction::on() const
{
    return m_on;
}

void GestureAction::setBlockOtherActions(const bool &blockOtherActions)
{
    m_blockOtherActions = blockOtherActions;
}

void GestureAction::setRepeatInterval(const ActionInterval &interval)
{
    m_interval = interval;
}

void GestureAction::setThresholds(const qreal &minimum, const qreal &maximum)
{
    m_minimumThreshold = minimum;
    m_maximumThreshold = maximum;
}

void GestureAction::setOn(const libgestures::On &on)
{
    m_on = on;
}

void GestureAction::reset()
{
    m_accumulatedDelta = 0;
    m_absoluteAccumulatedDelta = 0;
}

bool ActionInterval::matches(const qreal &value) const
{
    if (m_direction == IntervalDirection::Any) {
        return true;
    } else {
        return (value < 0 && m_direction == IntervalDirection::Negative) || (value > 0 && m_direction == IntervalDirection::Positive);
    }
}

const qreal &ActionInterval::value() const
{
    return m_value;
}

void ActionInterval::setValue(const qreal &value)
{
    m_value = value;
}

void ActionInterval::setDirection(const IntervalDirection &direction)
{
    m_direction = direction;
}

}