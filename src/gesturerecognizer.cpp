/*
    Most of this code was taken from https://invent.kde.org/plasma/kwin/-/blob/v6.1.5/src/gestures.h?ref_type=tags

    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-FileCopyrightText: 2017 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "config/config.h"
#include "effect/effecthandler.h"
#include "gesturerecognizer.h"
#include "window.h"

bool GestureRecognizer::swipeGestureBegin(uint fingerCount)
{
    m_currentFingerCount = fingerCount;

    if (!m_activeSwipeGestures.isEmpty())
        return true;

    for (std::shared_ptr<Gesture> &gesture : Config::instance().gestures)
    {
        const std::shared_ptr<SwipeGesture> swipeGesture = std::dynamic_pointer_cast<SwipeGesture>(gesture);
        if (!swipeGesture)
            continue;

        if (swipeGesture->minimumFingers > fingerCount || swipeGesture->maximumFingers < fingerCount)
            continue;

        const auto activeWindow = KWin::effects->activeWindow();
        if (activeWindow && !gesture->windowRegex.pattern().isEmpty()
            && !(gesture->windowRegex.match(activeWindow->window()->resourceClass()).hasMatch()
                || gesture->windowRegex.match(activeWindow->window()->resourceName()).hasMatch()))
            continue;

        switch (swipeGesture->direction)
        {
            case KWin::SwipeDirection::Up:
            case KWin::SwipeDirection::Down:
                if (m_currentSwipeAxis == Axis::Horizontal) {
                    continue;
                }
                break;
            case KWin::SwipeDirection::Left:
            case KWin::SwipeDirection::Right:
                if (m_currentSwipeAxis == Axis::Vertical) {
                    continue;
                }
                break;
            case KWin::SwipeDirection::Invalid:
                Q_UNREACHABLE();
        }

        m_activeSwipeGestures << swipeGesture;
        gesture->started();
    }

    return false;
}

bool GestureRecognizer::swipeGestureUpdate(const QPointF &delta)
{
    m_currentDelta += delta;

    KWin::SwipeDirection direction; // Overall direction
    Axis swipeAxis;

    // Pick an axis for gestures so horizontal ones don't change to vertical ones without lifting fingers
    if (m_currentSwipeAxis == Axis::None)
    {
        if (std::abs(m_currentDelta.x()) >= std::abs(m_currentDelta.y()))
        {
            swipeAxis = Axis::Horizontal;
            direction = m_currentDelta.x() < 0 ? KWin::SwipeDirection::Left : KWin::SwipeDirection::Right;
        }
        else
        {
            swipeAxis = Axis::Vertical;
            direction = m_currentDelta.y() < 0 ? KWin::SwipeDirection::Up : KWin::SwipeDirection::Down;
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
            direction = m_currentDelta.y() < 0 ? KWin::SwipeDirection::Up : KWin::SwipeDirection::Down;
            break;
        case Axis::Horizontal:
            direction = m_currentDelta.x() < 0 ? KWin::SwipeDirection::Left : KWin::SwipeDirection::Right;
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

            if (gesture->direction != direction)
            {
                gesture->cancelled();
                it = m_activeSwipeGestures.erase(it);
                continue;
            }
            else if (i == 1 && gesture->triggerAfterReachingThreshold && gesture->thresholdReached(m_currentDelta))
            {
                gesture->triggered();
                m_activeSwipeGestures.erase(it);
                m_hasActiveTriggeredSwipeGesture = true;
                swipeGestureEnd(false);
                return true;
            }

            it++;
        }
    }

    return m_hasActiveTriggeredSwipeGesture || !m_activeSwipeGestures.isEmpty();
}

bool GestureRecognizer::swipeGestureCancelled()
{
    bool hadActiveGestures = !m_activeSwipeGestures.isEmpty();
    for (auto g : std::as_const(m_activeSwipeGestures))
        g->cancelled();

    m_activeSwipeGestures.clear();

    m_currentFingerCount = 0;
    m_currentDelta = QPointF(0, 0);
    m_currentSwipeAxis = Axis::None;

    return hadActiveGestures;
}

bool GestureRecognizer::swipeGestureEnd(bool resetHasActiveTriggeredGesture)
{
    bool hadActiveGestures = !m_activeSwipeGestures.isEmpty();
    const QPointF delta = m_currentDelta;
    for (const auto &gesture : std::as_const(m_activeSwipeGestures))
    {
        if (gesture->thresholdReached(delta))
            gesture->triggered();
        else
            gesture->cancelled();
    }
    m_activeSwipeGestures.clear();
    m_currentFingerCount = 0;
    m_currentDelta = QPointF(0, 0);
    m_currentSwipeAxis = Axis::None;
    if (resetHasActiveTriggeredGesture)
        m_hasActiveTriggeredSwipeGesture = false;

    return hadActiveGestures;
}

bool GestureRecognizer::pinchGestureBegin(uint fingerCount)
{
    m_currentFingerCount = fingerCount;
    if (!m_activePinchGestures.isEmpty())
        return true;

    for (const std::shared_ptr<Gesture> &gesture : Config::instance().gestures)
    {
        const std::shared_ptr<PinchGesture> pinchGesture = std::dynamic_pointer_cast<PinchGesture>(gesture);
        if (!pinchGesture)
            continue;

        if (gesture->minimumFingers > fingerCount || gesture->maximumFingers < fingerCount)
            continue;

        const auto activeWindow = KWin::effects->activeWindow();
        if (activeWindow && !gesture->windowRegex.pattern().isEmpty()
            && !(gesture->windowRegex.match(activeWindow->window()->resourceClass()).hasMatch()
                 || gesture->windowRegex.match(activeWindow->window()->resourceName()).hasMatch()))
            continue;

        // direction doesn't matter yet
        m_activePinchGestures << pinchGesture;
        gesture->started();
    }

    return !m_activePinchGestures.isEmpty();
}

bool GestureRecognizer::pinchGestureUpdate(qreal scale, qreal angleDelta, const QPointF &delta)
{
    Q_UNUSED(angleDelta)
    Q_UNUSED(delta)

    m_currentScale = scale;

    // Determine the direction of the swipe
    KWin::PinchDirection direction = scale < 1 ? KWin::PinchDirection::Contracting : KWin::PinchDirection::Expanding;

    // Eliminate wrong gestures (takes two iterations)
    for (int i = 0; i < 2; i++)
    {
        if (m_activePinchGestures.isEmpty()) {
            pinchGestureBegin(m_currentFingerCount);
        }

        for (auto it = m_activePinchGestures.begin(); it != m_activePinchGestures.end();)
        {
            const auto &gesture = *it;
            if (gesture->direction != direction)
            {
                gesture->cancelled();
                it = m_activePinchGestures.erase(it);
                continue;
            }
            else if (gesture->triggerAfterReachingThreshold && gesture->thresholdReached(m_currentScale))
            {
                gesture->triggered();
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
    for (const auto &gesture : std::as_const(m_activePinchGestures))
        gesture->cancelled();

    m_activePinchGestures.clear();
    m_currentFingerCount = 0;
    m_currentScale = 1;

    return hadActiveGestures;
}

bool GestureRecognizer::pinchGestureEnd()
{
    bool hadActiveGestures = !m_activePinchGestures.isEmpty();
    for (const auto &gesture : std::as_const(m_activePinchGestures))
    {
        if (gesture->thresholdReached(m_currentScale))
            gesture->triggered();
        else
            gesture->cancelled();
    }

    m_activeSwipeGestures.clear();
    m_activePinchGestures.clear();
    m_currentScale = 1;
    m_currentFingerCount = 0;
    m_currentSwipeAxis = Axis::None;

    return hadActiveGestures;
}