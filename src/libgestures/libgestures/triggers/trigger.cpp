/*
    Input Actions - Input handler that executes user-defined actions
    Copyright (C) 2024-2025 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "trigger.h"

Q_LOGGING_CATEGORY(LIBGESTURES_TRIGGER, "libgestures.trigger", QtWarningMsg)

namespace libgestures
{

void Trigger::addAction(std::unique_ptr<GestureAction> action)
{
    m_actions.push_back(std::move(action));
}

void Trigger::setCondition(const std::shared_ptr<const Condition> &condition)
{
    m_condition = condition;
}

bool Trigger::canActivate(const TriggerActivationEvent *event) const
{
    if ((m_fingers && event->fingers && !m_fingers->contains(*event->fingers))
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

    qCDebug(LIBGESTURES_TRIGGER).noquote() << QString("Trigger updated (name: %1, delta: %2)").arg(m_name, QString::number(event->delta()));

    if (!m_started) {
        qCDebug(LIBGESTURES_TRIGGER).noquote() << QString("Trigger started (name: %1)").arg(m_name);
        m_started = true;
        for (const auto &action : m_actions) {
            action->gestureStarted();
        }
    }

    updateActions(event);
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

const std::vector<GestureAction *> Trigger::actions()
{
    std::vector<GestureAction *> result;
    for (auto &action : m_actions) {
        result.push_back(action.get());
    }
    return result;
}

void Trigger::updateActions(const TriggerUpdateEvent *event)
{
    for (const auto &action : m_actions) {
        action->gestureUpdated(event->delta(), {});
    }
}

void Trigger::setFingers(const Range<uint8_t> &fingers)
{
    m_fingers = fingers;
}

void Trigger::setBeginPositions(const std::vector<Range<QPointF>> &positions)
{
    m_beginPositions = positions;
}

void Trigger::setEndPositions(const std::vector<Range<QPointF>> &positions)
{
    m_endPositions = positions;
}

void Trigger::setThreshold(const Range<qreal> &threshold)
{
    m_threshold = threshold;
}

const std::optional<Qt::KeyboardModifiers> &Trigger::keyboardModifiers() const
{
    return m_keyboardModifiers;
}

void Trigger::setKeyboardModifiers(const Qt::KeyboardModifiers &modifiers)
{
    m_keyboardModifiers = modifiers;
}

const std::optional<Qt::MouseButtons> &Trigger::mouseButtons() const
{
    return m_mouseButtons;
}

void Trigger::setMouseButtons(const std::optional<Qt::MouseButtons> &buttons)
{
    m_mouseButtons = buttons;
}

const QString &Trigger::name() const
{
    return m_name;
}

void Trigger::setName(const QString &name)
{
    m_name = name;
}

const TriggerType &Trigger::type() const
{
    return m_type;
}

void Trigger::setType(const TriggerType &type)
{
    m_type = type;
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

const qreal &TriggerUpdateEvent::delta() const
{
    return m_delta;
}

void TriggerUpdateEvent::setDelta(const qreal &delta)
{
    m_delta = delta;
}

}