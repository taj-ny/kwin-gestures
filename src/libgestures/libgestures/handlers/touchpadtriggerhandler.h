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

#include "libgestures/handlers/multitouchmotiontriggerhandler.h"

namespace libgestures
{

/**
 * Handles touchpad triggers: pinch, press, rotate, stroke, swipe.
 *
 * This handler follows libinput's gesture lifecycle, making it not possible to for example ignore finger count
 * changes.
 */
class TouchpadTriggerHandler : public MultiTouchMotionTriggerHandler
{
public:
    TouchpadTriggerHandler();

    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool holdBegin(const uint8_t &fingers);
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool holdEnd();
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool holdCancel();

    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool pinchBegin(const uint8_t &fingers);
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool pinchUpdate(const qreal &scale, const qreal &angleDelta);
    /**
      * Handles an event. Called by the input collector.
      * @return Whether the event should be blocked.
      */
    bool pinchEnd();
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool pinchCancel();

    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool swipeBegin(const uint8_t &fingers);
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool swipeUpdate(const QPointF &delta);
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool swipeEnd();
    /**
     * Handles an event. Called by the input collector.
     * @return Whether the event should be blocked.
     */
    bool swipeCancel();

    /**
     * Handles an event. Called by the input collector.
     * The event is treated as a 2-finger swipe. Will not work if edge scrolling is enabled. The handler is not aware
     * when the finger count changes, therefore it relies on a timeout to end triggers.
     * @return Whether the input event should be blocked.
     * @see setScrollTimeout
     */
    bool scroll(const qreal &delta, const Qt::Orientation &orientation, const qreal &inverted);

    /**
     * Used in input actions, as KWin doesn't provide accelerated deltas for gestures. Temporary workaround.
     */
    void setSwipeDeltaMultiplier(const qreal &multiplier);
    /**
     * The time of inactivity in milliseconds after which 2-finger motion triggers will end.
     */
    void setScrollTimeout(const uint32_t &timeout);

private:
    uint32_t m_scrollTimeout = 100;
    QTimer m_scrollTimeoutTimer;
};

}