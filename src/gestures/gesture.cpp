#include "effect/effecthandler.h"
#include "gesture.h"
#include "window.h"

Gesture::Gesture(InputDeviceType device, bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, QRegularExpression windowRegex)
    : m_device(device),
      m_triggerWhenThresholdReached(triggerWhenThresholdReached),
      m_minimumFingers(minimumFingers),
      m_maximumFingers(maximumFingers),
      m_windowRegex(std::move(windowRegex))
{
}

void Gesture::cancelled()
{
    for (const auto &action : m_cancelledActions)
        action->execute();
}

void Gesture::started()
{
    for (const auto &action : m_startedActions)
        action->execute();
}

void Gesture::triggered()
{
    for (const auto &action : m_triggerActions)
        action->execute();
}

bool Gesture::meetsConditions(uint fingerCount) const
{
    if (m_minimumFingers > fingerCount || m_maximumFingers < fingerCount)
        return false;

    const auto activeWindow = KWin::effects->activeWindow();
    if (m_windowRegex.pattern().isEmpty())
        return true;

    return !activeWindow || (m_windowRegex.match(activeWindow->window()->resourceClass()).hasMatch()
                             || m_windowRegex.match(activeWindow->window()->resourceName()).hasMatch());
}

void Gesture::addTriggerAction(std::unique_ptr<GestureAction> action)
{
    m_triggerActions.push_back(std::move(action));
}