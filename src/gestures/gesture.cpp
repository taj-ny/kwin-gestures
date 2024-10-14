#include "effect/effecthandler.h"
#include "gesture.h"
#include "window.h"

Gesture::Gesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly)
    : m_device(device),
      m_triggerWhenThresholdReached(triggerWhenThresholdReached),
      m_minimumFingers(minimumFingers),
      m_maximumFingers(maximumFingers),
      m_triggerOneActionOnly(triggerOneActionOnly)
{
}

void Gesture::triggered()
{
    for (const auto &action : m_triggerActions)
    {
        if (!action->satisfiesConditions())
            continue;

        action->execute();
        if (m_triggerOneActionOnly)
            break;
    }
}

bool Gesture::satisfiesConditions(uint fingerCount) const
{
    if (m_minimumFingers > fingerCount || m_maximumFingers < fingerCount)
        return false;

    const bool satisfiesConditions = std::find_if(m_conditions.begin(), m_conditions.end(), [](const Condition &condition)
    {
        return condition.isSatisfied();
    }) != m_conditions.end();
    if (!m_conditions.empty() && !satisfiesConditions)
        return false;

    const bool actionSatisfiesConditions = std::find_if(m_triggerActions.begin(), m_triggerActions.end(), [](const std::shared_ptr<const GestureAction> &triggerAction)
    {
        return triggerAction->satisfiesConditions();
    }) != m_triggerActions.end();
    return m_triggerActions.empty() || actionSatisfiesConditions;
}

void Gesture::addTriggerAction(const std::shared_ptr<const GestureAction> &action)
{
    m_triggerActions.push_back(action);
}

void Gesture::addCondition(const Condition &condition)
{
    m_conditions.push_back(condition);
}