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

#include "motiontriggerhandler.h"

#include "libgestures/triggers/stroketrigger.h"

Q_LOGGING_CATEGORY(LIBGESTURES_HANDLER_MOTION, "libgestures.handler.motion", QtWarningMsg)

namespace libgestures
{

MotionTriggerHandler::MotionTriggerHandler()
{
    registerTriggerEndHandler(TriggerType::Stroke, std::bind(&MotionTriggerHandler::strokeTriggerEndHandler, this, std::placeholders::_1));
}

bool MotionTriggerHandler::updateMotion(const QPointF &delta)
{
    if (!hasActiveTriggers(TriggerType::StrokeSwipe)) {
        return false;
    }

    qCDebug(LIBGESTURES_HANDLER_MOTION).nospace() << "Event (type: Motion, delta: " << delta << ")";

    const auto hasStroke = hasActiveTriggers(TriggerType::Stroke);
    if (hasStroke) {
        m_stroke.push_back(delta);
    }
    m_currentSwipeDelta += delta;

    const auto deltaTotal = std::hypot(delta.x(), delta.y());
    if (!determineSpeed(deltaTotal, 0 /* TODO */)) {
        qCDebug(LIBGESTURES_HANDLER_MOTION, "Event processed (type: Motion, status: DeterminingSpeed)");
        return true;
    }

    std::map<TriggerType, const TriggerUpdateEvent *> events;
    DirectionalMotionTriggerUpdateEvent swipeEvent;
    MotionTriggerUpdateEvent strokeEvent;

    if (hasActiveTriggers(TriggerType::Swipe)) {
        SwipeDirection direction; // Overall direction
        Axis swipeAxis;

        // Pick an axis for gestures so horizontal ones don't change to vertical ones without lifting fingers
        if (m_currentSwipeAxis == Axis::None) {
            if (std::abs(m_currentSwipeDelta.x()) >= std::abs(m_currentSwipeDelta.y()))
                swipeAxis = Axis::Horizontal;
            else
                swipeAxis = Axis::Vertical;

            if (std::abs(m_currentSwipeDelta.x()) >= 5 || std::abs(m_currentSwipeDelta.y()) >= 5) {
                // only lock in a direction if the delta is big enough
                // to prevent accidentally choosing the wrong direction
                m_currentSwipeAxis = swipeAxis;
            }
        } else
            swipeAxis = m_currentSwipeAxis;

        // Find the current swipe direction
        switch (swipeAxis) {
            case Axis::Vertical:
                direction = m_currentSwipeDelta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
                break;
            case Axis::Horizontal:
                direction = m_currentSwipeDelta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
                break;
            default:
                Q_UNREACHABLE();
        }

        swipeEvent.setDelta(swipeAxis == Axis::Vertical ? delta.y() : delta.x());
        swipeEvent.setDirection(static_cast<uint32_t>(direction));
        swipeEvent.setDeltaMultiplied(delta * m_swipeDeltaMultiplier);
        swipeEvent.setSpeed(m_speed);
        events[TriggerType::Swipe] = &swipeEvent;
    }

    if (hasStroke) {
        strokeEvent.setDelta(deltaTotal);
        strokeEvent.setSpeed(m_speed);
        events[TriggerType::Stroke] = &strokeEvent;
    }

    const auto hasTriggers = updateTriggers(events);
    qCDebug(LIBGESTURES_HANDLER).nospace() << "Event processed (type: Motion, hasTriggers: " << hasTriggers << ")";
    return hasTriggers;
}

bool MotionTriggerHandler::determineSpeed(const qreal &delta, const qreal &fastThreshold)
{
    if (!m_isDeterminingSpeed) {
        return true;
    }

    if (m_sampledInputEvents++ != m_inputEventsToSample) {
        m_accumulatedAbsoluteSampledDelta += delta;
        qCDebug(LIBGESTURES_HANDLER)
            << qPrintable(QString("Determining speed (event: %1/%2, delta: %3/%4)")
                                  .arg(QString::number(m_sampledInputEvents), QString::number(m_inputEventsToSample), QString::number(m_accumulatedAbsoluteSampledDelta), QString::number(fastThreshold)));
        return false;
    }

    m_isDeterminingSpeed = false;
    m_speed = (m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= fastThreshold
        ? TriggerSpeed::Fast
        : TriggerSpeed::Slow;
    qCDebug(LIBGESTURES_HANDLER).noquote() << "Speed determined (speed: " << m_speed << ")";
    return true;
}

void MotionTriggerHandler::triggerActivating(const Trigger *trigger)
{
    if (const auto motionTrigger = dynamic_cast<const MotionTrigger *>(trigger)) {
        if (!m_isDeterminingSpeed && motionTrigger->hasSpeed()) {
            qCDebug(LIBGESTURES_HANDLER).noquote() << QString("Trigger has speed (name: %1)").arg(trigger->name());
            m_isDeterminingSpeed = true;
        }
    }
}

void MotionTriggerHandler::reset()
{
    TriggerHandler::reset();
    m_currentSwipeAxis = Axis::None;
    m_currentSwipeDelta = {};
    m_swipeDeltaMultiplier = 1.0;
    m_speed = TriggerSpeed::Any;
    m_isDeterminingSpeed = false;
    m_sampledInputEvents = 0;
    m_accumulatedAbsoluteSampledDelta = 0;
    m_stroke.clear();
}

void MotionTriggerHandler::strokeTriggerEndHandler(const TriggerEndEvent *event)
{
    if (m_stroke.empty()) {
        return;
    }

    const Stroke stroke(m_stroke);
    qCDebug(LIBGESTURES_HANDLER).noquote()
        << QString( "Stroke constructed (points: %1, deltas: %2)").arg(QString::number(stroke.points().size()), QString::number(m_stroke.size()));

    Trigger *best = nullptr;
    double bestScore = 0;
    for (const auto &trigger : activeTriggers(TriggerType::Stroke)) {
        if (!trigger->canEnd(event)) {
            continue;
        }

        for (const auto &triggerStroke : dynamic_cast<StrokeTrigger *>(trigger)->strokes()) {
            const auto score = stroke.compare(triggerStroke);
            if (score > bestScore && score > Stroke::min_matching_score()) {
                best = trigger;
                bestScore = score;
            }
        }
    }
    qCDebug(LIBGESTURES_HANDLER).noquote()
        << QString("Stroke compared (bestScore: %2)").arg(QString::number(bestScore));

    if (best) {
        cancelTriggers(best);
        best->end();
    }
    cancelTriggers(TriggerType::Stroke); // TODO Double cancellation
}

}