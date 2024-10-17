#include "gesture.h"

namespace libgestures
{

Gesture::Gesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold)
    : m_triggerWhenThresholdReached(triggerWhenThresholdReached),
      m_minimumFingers(minimumFingers),
      m_maximumFingers(maximumFingers),
      m_triggerOneActionOnly(triggerOneActionOnly),
      m_threshold(threshold)
{
    connect(this, &Gesture::cancelled, this, &Gesture::onCancelled);
    connect(this, &Gesture::ended, this, &Gesture::onEnded);
    connect(this, &Gesture::started, this, &Gesture::onStarted);
    connect(this, &Gesture::updated, this, &Gesture::onUpdated);
}

void Gesture::onCancelled()
{
    if (!m_hasStarted)
        return;

    for (auto &action : m_actions)
        action->cancelled();

    m_accumulatedDelta = 0;
    m_hasStarted = false;
}

void Gesture::onEnded()
{
    if (!m_hasStarted)
        return;

    for (const auto &action : m_actions)
        action->ended();

    m_accumulatedDelta = 0;
    m_hasStarted = false;
}

void Gesture::onStarted()
{
    m_hasStarted = true;

    for (const auto &action : m_actions)
        action->started();
}

void Gesture::onUpdated(const qreal &delta, bool &endedPrematurely)
{
    m_accumulatedDelta += delta;

    if (!m_hasStarted && !thresholdReached(m_accumulatedDelta))
        return;
    else if (!m_hasStarted)
        Q_EMIT started();

    for (const auto &action : m_actions)
    {
        if (!action->satisfiesConditions())
            continue;

        if (!action->repeat() && m_triggerWhenThresholdReached && action->canExecute()) {
            action->execute();
            if (m_triggerOneActionOnly)
            {
                Q_EMIT ended();
                endedPrematurely = true;
                return;
            }
        }

        action->update(delta);
    }
}

bool Gesture::satisfiesConditions(const uint8_t &fingerCount) const
{
    if (m_minimumFingers > fingerCount || m_maximumFingers < fingerCount)
        return false;

    const bool satisfiesConditions = std::find_if(m_conditions.begin(), m_conditions.end(), [](const Condition &condition)
    {
        return condition.isSatisfied();
    }) != m_conditions.end();
    if (!m_conditions.empty() && !satisfiesConditions)
        return false;

    const bool actionSatisfiesConditions = std::find_if(m_actions.begin(), m_actions.end(), [](const std::shared_ptr<const GestureAction> &triggerAction)
    {
        return triggerAction->satisfiesConditions();
    }) != m_actions.end();
    return m_actions.empty() || actionSatisfiesConditions;
}

bool Gesture::thresholdReached(const qreal &accumulatedDelta) const
{
    return accumulatedDelta >= m_threshold;
}

void Gesture::addAction(std::shared_ptr<GestureAction> action)
{
    m_actions.push_back(action);
}

void Gesture::addCondition(const Condition &condition)
{
    m_conditions.push_back(condition);
}

} // namespace libgestures