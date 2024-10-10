/*
    Most of this code was taken from https://invent.kde.org/plasma/kwin/-/blob/v6.1.5/src/gestures.h?ref_type=tags

    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gesturerecognizer.h"

namespace KWin
{

GestureRecognizer::~GestureRecognizer()
{
    unregisterGestures();
}

void GestureRecognizer::registerGesture(SwipeGesture *gesture)
{
    m_swipeGestures.append(gesture);
}

void GestureRecognizer::registerGesture(PinchGesture *gesture)
{
    m_pinchGestures.append(gesture);
}

void GestureRecognizer::unregisterGestures()
{
    for (SwipeGesture *swipeGesture : m_swipeGestures) {
        delete swipeGesture;
    }
    m_activeSwipeGestures.clear();
    m_swipeGestures.clear();
}

bool GestureRecognizer::swipeGestureBegin(uint fingerCount)
{
    m_currentFingerCount = fingerCount;

    if (!m_activeSwipeGestures.isEmpty())
        return true;

    for (SwipeGesture *gesture : std::as_const(m_swipeGestures))
    {
        if ((gesture->minimumFingerCount() != -1)
            && (gesture->minimumFingerCount() > fingerCount || gesture->maximumFingerCount() < fingerCount))
            continue;

        switch (gesture->direction())
        {
            case SwipeDirection::Up:
            case SwipeDirection::Down:
                if (m_currentSwipeAxis == Axis::Horizontal) {
                    continue;
                }
                break;
            case SwipeDirection::Left:
            case SwipeDirection::Right:
                if (m_currentSwipeAxis == Axis::Vertical) {
                    continue;
                }
                break;
            case SwipeDirection::Invalid:
                Q_UNREACHABLE();
        }

        m_activeSwipeGestures << gesture;
        Q_EMIT gesture->started();
    }

    return !m_activeSwipeGestures.isEmpty();
}

bool GestureRecognizer::swipeGestureUpdate(const QPointF &delta)
{
    m_currentDelta += delta;

    SwipeDirection direction; // Overall direction
    Axis swipeAxis;

    // Pick an axis for gestures so horizontal ones don't change to vertical ones without lifting fingers
    if (m_currentSwipeAxis == Axis::None)
    {
        if (std::abs(m_currentDelta.x()) >= std::abs(m_currentDelta.y()))
        {
            swipeAxis = Axis::Horizontal;
            direction = m_currentDelta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
        }
        else
        {
            swipeAxis = Axis::Vertical;
            direction = m_currentDelta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
        }

        if (std::abs(m_currentDelta.x()) >= 5 || std::abs(m_currentDelta.y()) >= 5)
        {
            // only lock in a direction if the delta is big enough
            // to prevent accidentally choosing the wrong direction
            m_currentSwipeAxis = swipeAxis;
        }
    }
    else
    {
        swipeAxis = m_currentSwipeAxis;
    }

    // Find the current swipe direction
    switch (swipeAxis)
    {
        case Axis::Vertical:
            direction = m_currentDelta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
            break;
        case Axis::Horizontal:
            direction = m_currentDelta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
            break;
        default:
            Q_UNREACHABLE();
    }

    // Eliminate wrong gestures (takes two iterations)
    for (int i = 0; i < 2; i++)
    {
        if (m_activeSwipeGestures.isEmpty())
            swipeGestureBegin(m_currentFingerCount);

        for (auto it = m_activeSwipeGestures.begin(); it != m_activeSwipeGestures.end();)
        {
            const auto gesture = *it;

            if (gesture->direction() != direction)
            {
                Q_EMIT gesture->cancelled();
                it = m_activeSwipeGestures.erase(it);
                continue;
            }
            else if (gesture->triggerAfterReachingThreshold() && gesture->thresholdReached(m_currentDelta))
            {
                Q_EMIT gesture->triggered();
                m_activeSwipeGestures.erase(it);
                pinchGestureEnd();
                break;
            }

            it++;
        }
    }

    return !m_activeSwipeGestures.isEmpty();
}

bool GestureRecognizer::swipeGestureCancelled()
{
    bool hadActiveGestures = !m_activeSwipeGestures.isEmpty();
    for (auto g : std::as_const(m_activeSwipeGestures))
    {
        Q_EMIT g->cancelled();
    }
    m_activeSwipeGestures.clear();

    m_currentFingerCount = 0;
    m_currentDelta = QPointF(0, 0);
    m_currentSwipeAxis = Axis::None;

    return hadActiveGestures;
}

bool GestureRecognizer::swipeGestureEnd()
{
    bool hadActiveGestures = !m_activeSwipeGestures.isEmpty();
    const QPointF delta = m_currentDelta;
    for (const auto &gesture : std::as_const(m_activeSwipeGestures))
    {
        if (gesture->thresholdReached(delta))
        {
            Q_EMIT gesture->triggered();
        }
        else
        {
            Q_EMIT gesture->cancelled();
        }
    }
    m_activeSwipeGestures.clear();
    m_currentFingerCount = 0;
    m_currentDelta = QPointF(0, 0);
    m_currentSwipeAxis = Axis::None;

    return hadActiveGestures;
}

bool GestureRecognizer::pinchGestureBegin(int fingerCount)
{
    m_currentFingerCount = fingerCount;
    if (!m_activePinchGestures.isEmpty())
        return true;

    for (PinchGesture *gesture : std::as_const(m_pinchGestures))
    {
        if ((gesture->minimumFingerCount() != -1)
            && (gesture->minimumFingerCount() > fingerCount || gesture->maximumFingerCount() < fingerCount))
            continue;

        // direction doesn't matter yet
        m_activePinchGestures << gesture;
        Q_EMIT gesture->started();
    }

    return !m_activePinchGestures.isEmpty();
}

bool GestureRecognizer::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta)
{
    m_currentScale = scale;

    // Determine the direction of the swipe
    PinchDirection direction = scale < 1 ? PinchDirection::Contracting : PinchDirection::Expanding;

    // Eliminate wrong gestures (takes two iterations)
    for (int i = 0; i < 2; i++)
    {
        if (m_activePinchGestures.isEmpty()) {
            pinchGestureBegin(m_currentFingerCount);
        }

        for (auto it = m_activePinchGestures.begin(); it != m_activePinchGestures.end();)
        {
            const auto gesture = *it;
            if (gesture->direction() != direction)
            {
                Q_EMIT gesture->cancelled();
                it = m_activePinchGestures.erase(it);
                continue;
            }
            else if (gesture->triggerAfterReachingThreshold() && gesture->thresholdReached(m_currentScale))
            {
                Q_EMIT gesture->triggered();
                m_activePinchGestures.erase(it);
                pinchGestureEnd();
                break;
            }
            it++;
        }
    }

    return !m_activePinchGestures.isEmpty();
}

bool GestureRecognizer::pinchGestureCancelled()
{
    bool hadActiveGestures = !m_activePinchGestures.isEmpty();
    for (const auto gesture : std::as_const(m_activePinchGestures))
        Q_EMIT gesture->cancelled();

    m_activePinchGestures.clear();
    m_currentFingerCount = 0;
    m_currentScale = 1;

    return hadActiveGestures;
}

bool GestureRecognizer::pinchGestureEnd()
{
    bool hadActiveGestures = !m_activePinchGestures.isEmpty();
    for (const auto gesture : std::as_const(m_activePinchGestures))
    {
        if (gesture->thresholdReached(m_currentScale))
            Q_EMIT gesture->triggered();
        else
            Q_EMIT gesture->cancelled();
    }

    m_activeSwipeGestures.clear();
    m_activePinchGestures.clear();
    m_currentScale = 1;
    m_currentFingerCount = 0;
    m_currentSwipeAxis = Axis::None;

    return hadActiveGestures;
}

};