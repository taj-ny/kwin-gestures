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

#pragma once

#include "libgestures/handlers/motiontriggerhandler.h"

namespace libgestures
{

/**
 * Handles mouse triggers: press, stroke, swipe, wheel
 */
class MouseTriggerHandler : public MotionTriggerHandler
{
public:
    MouseTriggerHandler();

    /**
     * @return Whether the input event should be blocked.
     */
    bool button(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state);
    void motion(const QPointF &delta);
    /**
     * @return Whether the input event should be blocked.
     */
    bool wheel(const qreal &delta, const Qt::Orientation &orientation);

    void setMotionTimeout(const qreal &timeout);
    void setPressTimeout(const qreal &timeout);

protected:
    /**
     * This implementation sets mouse buttons and position, which is the position of the pointer relative to the screen
     * it is on ranging from (0,0) to (1,1).
     *
     * @see TriggerHandler::createActivationEvent
     */
    std::unique_ptr<TriggerActivationEvent> createActivationEvent() const override;
    /**
    * This implementation sets the position.
    *
    * @see MouseTriggerHandler::createActivationEvent
    * @see TriggerHandler::createEndEvent
    */
    std::unique_ptr<TriggerEndEvent> createEndEvent() const override;

private:
    /**
     * Checks whether there is an activatable trigger that uses the specified button. Mouse buttons are ignored when
     * checking activatibility. If a trigger has multiple buttons, all of them will be blocked, even if only one was
     * pressed.
     */
    bool shouldBlockMouseButton(const Qt::MouseButton &button);
    /**
     * Presses all currently blocked mouse buttons without releasing them.
     */
    void pressBlockedMouseButtons();

    /**
     * Used to wait until all mouse buttons have been pressed to avoid conflicts with gestures that require more than
     * one button.
     */
    QTimer m_pressTimeoutTimer;
    qreal m_pressTimeout = 50;
    QTimer m_motionTimeoutTimer;
    qreal m_motionTimeout = 200;

    /**
     * Activation event for the last button press.
     */
    std::unique_ptr<TriggerActivationEvent> m_activationEvent;

    bool m_instantPress = false;
    qreal m_mouseMotionSinceButtonPress = 0;
    bool m_hadMouseGestureSinceButtonPress = false;
    QList<quint32> m_blockedMouseButtons;
};

}