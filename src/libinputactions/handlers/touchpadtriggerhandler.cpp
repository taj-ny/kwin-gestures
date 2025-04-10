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

#include "touchpadtriggerhandler.h"

namespace libinputactions
{

TouchpadTriggerHandler::TouchpadTriggerHandler()
{
    m_scrollTimeoutTimer.setSingleShot(true);
    connect(&m_scrollTimeoutTimer, &QTimer::timeout, this, [this] {
        endTriggers(TriggerType::StrokeSwipe);
    });
}

bool TouchpadTriggerHandler::handleHoldBeginEvent(const uint8_t &fingers)
{
    return activateTriggers(TriggerType::Press, fingers);
}

bool TouchpadTriggerHandler::handleHoldEndEvent()
{
    return endTriggers(TriggerType::Press);
}

bool TouchpadTriggerHandler::handleHoldCancelEvent()
{
    return cancelTriggers(TriggerType::Press);
}

bool TouchpadTriggerHandler::handlePinchBeginEvent(const uint8_t &fingers)
{
    return activateTriggers(TriggerType::PinchRotate, fingers);
}

bool TouchpadTriggerHandler::handlePinchUpdateEvent(const qreal &scale, const qreal &angleDelta)
{
    return handlePinch(scale, angleDelta);
}

bool TouchpadTriggerHandler::handlePinchEndEvent()
{
    return endTriggers(TriggerType::PinchRotate);
}

bool TouchpadTriggerHandler::handlePinchCancelEvent()
{
    return cancelTriggers(TriggerType::PinchRotate);
}

bool TouchpadTriggerHandler::handleSwipeBeginEvent(const uint8_t &fingers)
{
    return activateTriggers(TriggerType::StrokeSwipe, fingers);
}

bool TouchpadTriggerHandler::handleSwipeUpdateEvent(const QPointF &delta)
{
    return handleMotion(delta);
}

bool TouchpadTriggerHandler::handleSwipeEndEvent()
{
    return endTriggers(TriggerType::StrokeSwipe);
}

bool TouchpadTriggerHandler::handleSwipeCancelEvent()
{
    return cancelTriggers(TriggerType::StrokeSwipe);
}

bool TouchpadTriggerHandler::handleScrollEvent(const qreal &delta, const Qt::Orientation &orientation, const qreal &inverted)
{
    auto motionDelta = orientation == Qt::Orientation::Horizontal
        ? QPointF(delta, 0)
        : QPointF(0, delta);
    if (inverted) {
        motionDelta *= -1;
    }

    if (!m_scrollTimeoutTimer.isActive()) {
        activateTriggers(TriggerType::StrokeSwipe, 2);
    }
    m_scrollTimeoutTimer.start(m_scrollTimeout);
    if (handleMotion(motionDelta)) {
        return true;
    }
    m_scrollTimeoutTimer.stop();
    return false;
}

void TouchpadTriggerHandler::setScrollTimeout(const uint32_t &timeout)
{
    m_scrollTimeout = timeout;
}

}