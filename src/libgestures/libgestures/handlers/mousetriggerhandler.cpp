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

#include "mousetriggerhandler.h"

#include "libgestures/triggers/presstrigger.h"

namespace libgestures
{

MouseTriggerHandler::MouseTriggerHandler()
    : MotionTriggerHandler()
{
    m_pressTimeoutTimer.setTimerType(Qt::TimerType::PreciseTimer);
    m_pressTimeoutTimer.setSingleShot(true);

    m_motionTimeoutTimer.setTimerType(Qt::TimerType::PreciseTimer);
    m_motionTimeoutTimer.setSingleShot(true);
}

bool MouseTriggerHandler::button(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state)
{
    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event (type: PointerMotion, button: " << button << ", state: " << state << ")";

    if (state) {
        m_mouseMotionSinceButtonPress = 0;
        m_hadMouseGestureSinceButtonPress = false;
        cancelTriggers(TriggerType::All);
        m_activationEvent = createActivationEvent();

        // This should be per-gesture instead of global, but it's good enough
        m_instantPress = false;
        for (const auto &trigger : triggers(TriggerType::Press, m_activationEvent.get())) {
            if (dynamic_cast<PressTrigger *>(trigger)->instant()) {
                qCDebug(LIBGESTURES_HANDLER, "Press gesture is instant");
                m_instantPress = true;
                break;
            }
        }

        disconnect(&m_pressTimeoutTimer, nullptr, nullptr, nullptr);
        disconnect(&m_motionTimeoutTimer, nullptr, nullptr, nullptr);
        connect(&m_pressTimeoutTimer, &QTimer::timeout, this, [this] {
            const auto swipeTimeout = [this] {
                if (m_hadMouseGestureSinceButtonPress) {
                    qCDebug(LIBGESTURES_HANDLER, "Mouse gesture updated before motion timeout");
                    return;
                }

                qCDebug(LIBGESTURES_HANDLER, "Attempting to activate mouse press gestures");
                if (!activateTriggers(TriggerType::Press, m_activationEvent.get())) {
                    qCDebug(LIBGESTURES_HANDLER, "No wheel or press mouse gestures");
                    pressBlockedMouseButtons();
                }
            };

            if (m_instantPress) {
                swipeTimeout();
                return;
            }

            connect(&m_motionTimeoutTimer, &QTimer::timeout, [swipeTimeout] {
                qCDebug(LIBGESTURES_HANDLER, "No mouse motion");
                swipeTimeout();
            });
            m_motionTimeoutTimer.start(m_motionTimeout);
            qCDebug(LIBGESTURES_HANDLER, "Waiting for mouse motion");
        });
        m_pressTimeoutTimer.start(m_pressTimeout);
        qCDebug(LIBGESTURES_HANDLER, "Waiting for all mouse buttons");

        if (shouldBlockMouseButton(button)) {
            m_blockedMouseButtons.push_back(nativeButton);
            return true;
        }
    } else {
        endTriggers(TriggerType::All);

        // Prevent gesture skipping when clicking rapidly
        if (m_pressTimeoutTimer.isActive() || m_motionTimeoutTimer.isActive()) {
            m_pressTimeoutTimer.stop();
            m_motionTimeoutTimer.stop();

            if (m_instantPress) {
                activateTriggers(TriggerType::Press, m_activationEvent.get());
                pressUpdate(5); // TODO
                endTriggers(TriggerType::Press);
            }
        }

        if (m_blockedMouseButtons.removeAll(nativeButton) && !m_hadMouseGestureSinceButtonPress) {
            qCDebug(LIBGESTURES_HANDLER).nospace() << "Mouse button pressed and released (button: " << nativeButton << ")";
            Input::implementation()->mouseButton(nativeButton, true);
            Input::implementation()->mouseButton(nativeButton, false);
        }

        if (m_blockedMouseButtons.empty()) {
            m_hadMouseGestureSinceButtonPress = false;
        }
    }

    return false;
}

void MouseTriggerHandler::motion(const QPointF &delta)
{
    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event (type: PointerMotion, delta: " << delta << ")";

    m_mouseMotionSinceButtonPress += std::hypot(delta.x(), delta.y());
    if (m_mouseMotionSinceButtonPress < 5) {
        qCDebug(LIBGESTURES_HANDLER).nospace() << "Insufficient movement to start mouse motion gestures (delta: " << delta << ")";
        return;
    }

    if (m_pressTimeoutTimer.isActive() || m_motionTimeoutTimer.isActive()) {
        cancelTriggers(TriggerType::All);

        m_pressTimeoutTimer.stop();
        m_motionTimeoutTimer.stop();

        qCDebug(LIBGESTURES_HANDLER, "Attempting to activate mouse motion gestures");
        if (!activateTriggers(TriggerType::StrokeSwipe)) {
            qCDebug(LIBGESTURES_HANDLER, "No motion gestures");
            pressBlockedMouseButtons();
        }
    } else if (!hasActiveTriggers(TriggerType::StrokeSwipe) && Input::implementation()->keyboardModifiers()) {
        qCDebug(LIBGESTURES_HANDLER, "Keyboard modifiers present, attempting to start mouse motion gestures");
        activateTriggers(TriggerType::StrokeSwipe);
    }

    const auto hadActiveGestures = hasActiveTriggers(TriggerType::StrokeSwipe);
    updateMotion(delta);
    if (hadActiveGestures && !hasActiveTriggers(TriggerType::StrokeSwipe)) {
        qCDebug(LIBGESTURES_HANDLER, "Mouse motion gesture ended/cancelled during motion");
        // Swipe gesture cancelled due to wrong speed or direction
        pressBlockedMouseButtons();
    }
}

bool MouseTriggerHandler::wheel(const qreal &delta, const Qt::Orientation &orientation)
{
    const auto motionDelta = orientation == Qt::Orientation::Horizontal
        ? QPointF(delta, 0)
        : QPointF(0, delta);
    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event (type: Wheel, delta: " << motionDelta << ")";

    if (!activateTriggers(TriggerType::Wheel)) {
        qCDebug(LIBGESTURES_HANDLER, "Event processed (type: Wheel, status: NoGestures)");
        return false;
    }

    SwipeDirection direction = SwipeDirection::Left;
    if (motionDelta.x() > 0) {
        direction = SwipeDirection::Right;
    } else if (motionDelta.y() > 0) {
        direction = SwipeDirection::Down;
    } else if (motionDelta.y() < 0) {
        direction = SwipeDirection::Up;
    }
    DirectionalMotionTriggerUpdateEvent event;
    event.setDelta(motionDelta.x() != 0 ? motionDelta.x() : motionDelta.y());
    event.setDirection(static_cast<uint32_t>(direction));
    updateTriggers(TriggerType::Wheel, &event);

    const auto hadTriggers = endTriggers(TriggerType::Wheel);
    qCDebug(LIBGESTURES_HANDLER).noquote().nospace() << "Event processed (type: Wheel, hasGestures: " << hadTriggers << ")";
    return hadTriggers;
}

void MouseTriggerHandler::setMotionTimeout(const qreal &timeout)
{
    m_motionTimeout = timeout;
}

void MouseTriggerHandler::setPressTimeout(const qreal &timeout)
{
    m_pressTimeout = timeout;
}

std::unique_ptr<TriggerActivationEvent> MouseTriggerHandler::createActivationEvent() const
{
    auto event = TriggerHandler::createActivationEvent();
    event->mouseButtons = Input::implementation()->mouseButtons();
    event->position = Input::implementation()->mousePosition();
    return event;
}

std::unique_ptr<TriggerEndEvent> MouseTriggerHandler::createEndEvent() const
{
    auto event = TriggerHandler::createEndEvent();
    event->position = Input::implementation()->mousePosition();
    return event;
}

bool MouseTriggerHandler::shouldBlockMouseButton(const Qt::MouseButton &button)
{
    const auto event = createActivationEvent();
    // Triggers support multiple buttons, so this property must be ignored when finding activatable triggers
    event->mouseButtons = std::nullopt;
    for (const auto &trigger : triggers(TriggerType::All, event.get())) {
        const auto buttons = trigger->mouseButtons();
        if (buttons && (*buttons & button)) {
            qCDebug(LIBGESTURES_HANDLER).noquote().nospace() << "Mouse button blocked (button: " << button << ", trigger: " << trigger->name() << ")";
            return true;
        }
    }
    return false;
}

void MouseTriggerHandler::pressBlockedMouseButtons()
{
    for (const auto &button : m_blockedMouseButtons) {
        Input::implementation()->mouseButton(button, true);
        qCDebug(LIBGESTURES_HANDLER).nospace() << "Mouse button unblocked (button: " << button << ")";
    }
    m_blockedMouseButtons.clear();
}

}