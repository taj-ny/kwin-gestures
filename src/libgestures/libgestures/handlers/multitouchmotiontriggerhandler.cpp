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

#include "multitouchmotiontriggerhandler.h"

namespace libgestures
{

bool MultiTouchMotionTriggerHandler::updatePinch(const qreal &scale, const qreal &angleDelta, const QPointF &delta)
{
    if (!hasActiveTriggers(TriggerType::PinchRotate)) {
        return false;
    }

    qCDebug(LIBGESTURES_HANDLER).nospace() << "Pinch event (scale: " << scale << ", angleDelta: " << angleDelta << ", delta: " << delta << ")";

    const auto pinchDelta = -(m_previousPinchScale - scale);
    m_previousPinchScale = scale;

    PinchDirection pinchDirection = scale < 1 ? PinchDirection::In : PinchDirection::Out;

    RotateDirection rotateDirection = angleDelta > 0 ? RotateDirection::Clockwise : RotateDirection::Counterclockwise;
    m_accumulatedRotateDelta += std::abs(angleDelta);

    if (m_pinchType == PinchType::Unknown) {
        if (m_accumulatedRotateDelta >= 10) {
            qCDebug(LIBGESTURES_HANDLER, "Determined pinch type (rotate)");
            m_pinchType = PinchType::Rotate;
            cancelTriggers(TriggerType::Pinch);
        } else if (std::abs(1.0 - scale) >= 0.2) {
            qCDebug(LIBGESTURES_HANDLER, "Determined pinch type (pinch)");
            m_pinchType = PinchType::Pinch;
            cancelTriggers(TriggerType::Rotate);
        } else {
            qCDebug(LIBGESTURES_HANDLER, "Event processed (type: Pinch, status: DeterminingType)");
            return true;
        }
    }

    const auto speedThreshold = 0.0f;
//    const auto speedThreshold = m_pinchType == PinchType::Rotate
//        ? m_rotateFastThreshold
//        : (scale < 1 ? m_pinchInFastThreshold : m_pinchOutFastThreshold);
    if (!determineSpeed(std::abs(m_pinchType == PinchType::Rotate ? angleDelta : pinchDelta), speedThreshold)) {
        qCDebug(LIBGESTURES_HANDLER, "Event processed (type: Pinch, status: DeterminingSpeed)");
        return true;
    }

    auto hasGestures = false;
    if (m_pinchType == PinchType::Rotate) {
        DirectionalMotionTriggerUpdateEvent event;
        event.setDelta(angleDelta);
        event.setDirection(static_cast<uint32_t>(rotateDirection));
        event.setSpeed(m_speed);
        hasGestures = updateTriggers(TriggerType::Rotate, &event);
    } else if (m_pinchType == PinchType::Pinch) {
        DirectionalMotionTriggerUpdateEvent event;
        event.setDelta(pinchDelta);
        event.setDirection(static_cast<uint32_t>(pinchDirection));
        event.setSpeed(m_speed);
        hasGestures = updateTriggers(TriggerType::Pinch, &event);
    }

    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event processed (type: Pinch, hasGestures: " << hasGestures << ")";
    return false;
}

void MultiTouchMotionTriggerHandler::reset()
{
    MotionTriggerHandler::reset();
    m_previousPinchScale = 1;
    m_pinchType = PinchType::Unknown;
    m_accumulatedRotateDelta = 0;
}


}