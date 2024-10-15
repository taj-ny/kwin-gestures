#include "effect/effecthandler.h"
#include "gesture.h"
#include "window.h"

Gesture::Gesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold)
    : m_triggerWhenThresholdReached(triggerWhenThresholdReached),
      m_minimumFingers(minimumFingers),
      m_maximumFingers(maximumFingers),
      m_triggerOneActionOnly(triggerOneActionOnly),
      m_threshold(threshold)
{
}

void Gesture::cancelled()
{
    if (!m_hasStarted)
        return;

    for (auto &action : m_actions)
        action->cancelled();

    m_accumulatedDelta = 0;
    m_hasStarted = false;
}

void Gesture::ended()
{
    if (!m_hasStarted)
        return;

    for (const auto &action : m_actions)
        action->ended();

    m_accumulatedDelta = 0;
    m_hasStarted = false;
}

void Gesture::start()
{
    m_hasStarted = true;

    for (const auto &action : m_actions)
        action->started();
}

bool Gesture::update(const qreal &delta)
{
    m_accumulatedDelta += delta;

    if (!m_hasStarted && !thresholdReached(m_accumulatedDelta))
        return false;
    else if (!m_hasStarted)
        start();

    for (const auto &action : m_actions)
    {
        if (!action->satisfiesConditions())
            continue;

        if (!action->repeat() && m_triggerWhenThresholdReached && action->canExecute()) {
            action->execute();
            if (m_triggerOneActionOnly)
            {
                ended();
                return true;
            }
        }

        action->update(delta);
    }

    return false;
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