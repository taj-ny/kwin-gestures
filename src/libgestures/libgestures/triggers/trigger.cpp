#include "trigger.h"

Q_LOGGING_CATEGORY(LIBGESTURES_TRIGGER, "libgestures.trigger", QtWarningMsg)

namespace libgestures
{

bool Trigger::canActivate(const TriggerActivateEvent *event) const
{
    if ((event->fingers && !m_fingers.contains(*event->fingers))
        || (m_keyboardModifiers && event->keyboardModifiers && *m_keyboardModifiers != event->keyboardModifiers)
        || (m_mouseButtons && event->mouseButtons && *m_mouseButtons != event->mouseButtons)
        || (m_beginPositions && event->position && std::find_if(m_beginPositions->begin(), m_beginPositions->end(), [&event](const auto &position) {
        return position.contains(*event->position);
    }) == m_beginPositions->end())) {
        return false;
    }

    return !m_condition || m_condition.value()->satisfied();
}

bool Trigger::canUpdate(const TriggerUpdateEvent *) const
{
    return true;
}

void Trigger::update(const TriggerUpdateEvent *event)
{
    m_absoluteAccumulatedDelta += std::abs(event->delta());
    m_thresholdReached = thresholdReached();
    if (!m_thresholdReached) {
        qCDebug(LIBGESTURES_TRIGGER).noquote()
            << QString("Threshold not reached (name: %1, current: %2, min: %3, max: %4")
                .arg(m_name, QString::number(m_absoluteAccumulatedDelta), QString::number(m_threshold->min()), QString::number(m_threshold->max()));
        return;
    }

    qCDebug(LIBGESTURES_TRIGGER).noquote() << QString("Trigger updated (name: %1, delta: %2)").arg(m_name, QString::number(event->delta));

    if (!m_started) {
        qCDebug(LIBGESTURES_TRIGGER).noquote() << QString("Trigger started (name: %1)").arg(m_name);
        m_started = true;
        for (const auto &action : m_actions) {
            action->gestureStarted();
        }
    }
    /* TODO
    for (const auto &action : m_actions) {
        action->gestureUpdated(delta, deltaPointMultiplied);
        if (action->blocksOtherActions()) {
            end();
            return false;
        }
    }
     */

    updateDerived(event);
}

bool Trigger::canEnd(const TriggerEndEvent *event) const
{
    if (!m_endPositions || !event->position) {
        return true;
    }

    return std::find_if(m_endPositions->begin(), m_endPositions->end(), [event](const auto &position) {
        return position.contains(event->position.value());
    }) == m_endPositions->end();
}

void Trigger::end()
{
    qCDebug(LIBGESTURES_TRIGGER).noquote() << QString("Trigger cancelled (name: %1)").arg(m_name);
    for (const auto &action : m_actions) {
        action->gestureEnded(m_thresholdReached);
    }
    reset();
}

void Trigger::cancel()
{
    qCDebug(LIBGESTURES_TRIGGER).noquote() << QString("Trigger ended (name: %1)").arg(m_name);
    for (const auto &action : m_actions) {
        action->gestureCancelled(m_thresholdReached);
    }
    reset();
}

bool Trigger::overridesOtherTriggersOnEnd()
{
    if (!m_thresholdReached) {
        return false;
    }

    return std::any_of(m_actions.begin(), m_actions.end(), [](const auto &action) {
        return action->on() == On::End && action->canExecute();
    });
}

bool Trigger::overridesOtherTriggersOnUpdate()
{
    if (!m_thresholdReached) {
        return false;
    }

    return std::any_of(m_actions.begin(), m_actions.end(), [](const auto &action) {
        return action->executed() || (action->on() == On::Update && action->canExecute());
    });
}

void Trigger::updateDerived(const libgestures::TriggerUpdateEvent *event)
{
    for (const auto &action : m_actions) {
        action->gestureUpdated(event->delta, {});
    }
}

bool Trigger::thresholdReached() const
{
    if (!m_threshold) {
        return true;
    }

    return (m_threshold->min() == 0 || m_absoluteAccumulatedDelta >= m_threshold->min())
        && (m_threshold->max() == 0 || m_absoluteAccumulatedDelta <= m_threshold->max());

}

void Trigger::reset()
{
    m_started = false;
    m_absoluteAccumulatedDelta = 0;
    m_threshold = false;
}

}