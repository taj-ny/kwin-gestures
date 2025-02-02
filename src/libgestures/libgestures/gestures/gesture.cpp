#include "gesture.h"

namespace libgestures
{

Gesture::Gesture()
{
    connect(this, &Gesture::cancelled, this, &Gesture::onCancelled);
    connect(this, &Gesture::ended, this, &Gesture::onEnded);
    connect(this, &Gesture::started, this, &Gesture::onStarted);
    connect(this, &Gesture::updated, this, &Gesture::onUpdated);
}

void Gesture::onCancelled()
{
    m_absoluteAccumulatedDelta = 0;

    if (!m_hasStarted)
        return;

    m_hasStarted = false;

    for (const auto &action : m_actions)
    {
        bool actionExecuted = false;
        Q_EMIT action->gestureCancelled(actionExecuted);
        if (action->blocksOtherActions() && actionExecuted)
            break;
    }
}

void Gesture::onEnded()
{
    m_absoluteAccumulatedDelta = 0;

    if (!m_hasStarted)
        return;

    m_hasStarted = false;

    for (const auto &action : m_actions)
    {
        bool actionExecuted = false;
        Q_EMIT action->gestureEnded(actionExecuted);
        if (action->blocksOtherActions() && actionExecuted)
            break;
    }
}

void Gesture::onStarted()
{
    if (!m_hasStarted)
        return;

    for (const auto &action : m_actions)
    {
        bool actionExecuted = false;
        Q_EMIT action->gestureStarted(actionExecuted);
        if (action->blocksOtherActions() && actionExecuted)
            break;
    }
}

void Gesture::onUpdated(const qreal &delta, const QPointF &deltaPointMultiplied, bool &endedPrematurely)
{
    m_absoluteAccumulatedDelta += std::abs(delta);
    if (!thresholdReached())
        return;

    if (!m_hasStarted)
    {
        m_hasStarted = true;
        Q_EMIT started();
    }

    for (const auto &action : m_actions)
    {
        bool actionExecuted = false;
        Q_EMIT action->gestureUpdated(delta, deltaPointMultiplied, actionExecuted);
        if (actionExecuted && action->blocksOtherActions())
        {
            endedPrematurely = true;
            Q_EMIT ended();
            return;
        }
    }
}

bool Gesture::satisfiesConditions(const uint8_t &fingerCount) const
{
    if (m_minimumFingers > fingerCount || m_maximumFingers < fingerCount)
        return false;

    const bool satisfiesConditions = std::find_if(m_conditions.begin(), m_conditions.end(), [](const std::shared_ptr<const Condition> &condition)
    {
        return condition->isSatisfied();
    }) != m_conditions.end();
    if (!m_conditions.empty() && !satisfiesConditions)
        return false;

    const bool actionSatisfiesConditions = std::find_if(m_actions.begin(), m_actions.end(), [](const std::shared_ptr<const GestureAction> &triggerAction)
    {
        return triggerAction->canExecute();
    }) != m_actions.end();
    return m_actions.empty() || actionSatisfiesConditions;
}

void Gesture::addAction(const std::shared_ptr<GestureAction> &action)
{
    m_actions.push_back(action);
}

void Gesture::addCondition(const std::shared_ptr<const Condition> &condition)
{
    m_conditions.push_back(condition);
}

void Gesture::setThresholds(const qreal &minimum, const qreal &maximum)
{
    m_minimumThreshold = minimum;
    m_maximumThreshold = maximum;
}

void Gesture::setFingers(const uint8_t &minimum, const uint8_t &maximum)
{
    m_minimumFingers = minimum;
    m_maximumFingers = maximum;
}

bool Gesture::thresholdReached() const
{
    return ((m_minimumThreshold == -1 || m_absoluteAccumulatedDelta >= m_minimumThreshold)
        && (m_maximumThreshold == -1 || m_absoluteAccumulatedDelta <= m_maximumThreshold));
}

void Gesture::setSpeed(const libgestures::GestureSpeed &speed)
{
    m_speed = speed;
}

GestureSpeed Gesture::speed() const
{
    return m_speed;
}

}