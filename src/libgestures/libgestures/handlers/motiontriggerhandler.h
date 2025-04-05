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

#include "libgestures/triggers/directionalmotiontrigger.h"
#include "triggerhandler.h"

Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_HANDLER_MOTION)

namespace libgestures
{

enum class Axis
{
    Horizontal,
    Vertical,
    None
};

class MotionTriggerHandler : public TriggerHandler
{
public:
    MotionTriggerHandler();

    bool updateMotion(const QPointF &delta);

protected:
    bool determineSpeed(const qreal &delta, const qreal &fastThreshold);

    void triggerActivating(const Trigger *trigger) override;
    void reset() override;

    TriggerSpeed m_speed = TriggerSpeed::Any;

private:
    void strokeTriggerEndHandler(const TriggerEndEvent *event);

    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;
    qreal m_swipeDeltaMultiplier = 1.0;

    bool m_isDeterminingSpeed = false;
    uint8_t m_inputEventsToSample = 3;
    uint8_t m_sampledInputEvents = 0;
    qreal m_accumulatedAbsoluteSampledDelta = 0;

    std::vector<QPointF> m_stroke;
};

}