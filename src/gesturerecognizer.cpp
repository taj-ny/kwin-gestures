#include "config/config.h"
#include "effect/effecthandler.h"
#include "gesturerecognizer.h"
#include "window.h"
#include "input.h"
#include "input_event_spy.h"

void GestureRecognizer::registerGesture(std::shared_ptr<Gesture> gesture)
{
    m_gestures.push_back(gesture);
}

void GestureRecognizer::unregisterGestures()
{
    m_gestures.clear();
}

void GestureRecognizer::holdGestureBegin(const uint8_t &fingerCount)
{
    for (std::shared_ptr<Gesture> gesture : m_gestures)
    {
        std::shared_ptr<HoldGesture> holdGesture = std::dynamic_pointer_cast<HoldGesture>(gesture);
        if (!holdGesture || !gesture->satisfiesConditions(fingerCount))
            continue;

        m_activeHoldGestures << gesture;
    }
}

void GestureRecognizer::holdGestureUpdate(const qreal &delta, bool &endedPrematurely)
{
    for (const auto &holdGesture : m_activeHoldGestures)
    {
        if (!holdGesture->update(delta))
            continue;

        endedPrematurely = true;
        return;
    }
}

void GestureRecognizer::holdGestureCancelled()
{
    for (const auto &gesture : m_activeHoldGestures)
        gesture->cancelled();
    m_activeHoldGestures.clear();
}

bool GestureRecognizer::holdGestureEnd()
{
    const bool hadActiveGestures = !m_activeHoldGestures.isEmpty();
    for (const auto &gesture : m_activeHoldGestures)
        gesture->ended();
    m_activeHoldGestures.clear();
    return hadActiveGestures;
}

void GestureRecognizer::swipeGestureBegin(const uint8_t &fingerCount)
{
    m_currentFingerCount = fingerCount;

    if (!m_activeSwipeGestures.isEmpty())
        return;

    for (std::shared_ptr<Gesture> gesture : m_gestures)
    {
        const std::shared_ptr<SwipeGesture> swipeGesture = std::dynamic_pointer_cast<SwipeGesture>(gesture);
        if (!swipeGesture || !gesture->satisfiesConditions(fingerCount))
            continue;

        m_activeSwipeGestures << swipeGesture;
    }
}

bool GestureRecognizer::swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely)
{
    m_currentSwipeDelta += delta;

    SwipeDirection direction; // Overall direction
    Axis swipeAxis;

    // Pick an axis for gestures so horizontal ones don't change to vertical ones without lifting fingers
    if (m_currentSwipeAxis == Axis::None)
    {
        if (std::abs(m_currentSwipeDelta.x()) >= std::abs(m_currentSwipeDelta.y()))
            swipeAxis = Axis::Horizontal;
        else
            swipeAxis = Axis::Vertical;

        if (std::abs(m_currentSwipeDelta.x()) >= 5 || std::abs(m_currentSwipeDelta.y()) >= 5)
        {
            // only lock in a direction if the delta is big enough
            // to prevent accidentally choosing the wrong direction
            m_currentSwipeAxis = swipeAxis;
        }
    }
    else
        swipeAxis = m_currentSwipeAxis;

    // Find the current swipe direction
    switch (swipeAxis)
    {
        case Axis::Vertical:
            direction = m_currentSwipeDelta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
            break;
        case Axis::Horizontal:
            direction = m_currentSwipeDelta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
            break;
        default:
            Q_UNREACHABLE();
    }

    for (auto it = m_activeSwipeGestures.begin(); it != m_activeSwipeGestures.end();)
    {
        const auto gesture = *it;

        if (!((gesture->direction() == SwipeDirection::LeftRight
                && (direction == SwipeDirection::Left || direction == SwipeDirection::Right))
            || (gesture->direction() == SwipeDirection::UpDown
                && (direction == SwipeDirection::Up || direction == SwipeDirection::Down)))
            && gesture->direction() != direction)
        {
            gesture->cancelled();
            it = m_activeSwipeGestures.erase(it);
            continue;
        }
        else if (gesture->update(swipeAxis == Axis::Vertical ? delta.y() : delta.x()))
        {
            endedPrematurely = true;
            return true;
        }

        it++;
    }

    return !m_activeSwipeGestures.isEmpty();
}

void GestureRecognizer::swipeGestureCancelled()
{
    for (const auto &gesture : m_activeSwipeGestures)
        gesture->cancelled();
    m_activeSwipeGestures.clear();

    m_currentFingerCount = 0;
    m_currentSwipeAxis = Axis::None;
    m_currentSwipeDelta = QPointF();
}

bool GestureRecognizer::swipeGestureEnd()
{
    bool hadActiveGestures = !m_activeSwipeGestures.isEmpty();
    for (const auto &gesture : m_activeSwipeGestures)
        gesture->ended();
    m_activeSwipeGestures.clear();

    m_currentFingerCount = 0;
    m_currentSwipeAxis = Axis::None;
    m_currentSwipeDelta = QPointF();

    return hadActiveGestures;
}

void GestureRecognizer::pinchGestureBegin(const uint8_t &fingerCount)
{
    m_currentFingerCount = fingerCount;
    if (!m_activePinchGestures.isEmpty())
        return;

    for (const std::shared_ptr<Gesture> &gesture : m_gestures)
    {
        const std::shared_ptr<PinchGesture> pinchGesture = std::dynamic_pointer_cast<PinchGesture>(gesture);
        if (!pinchGesture || !gesture->satisfiesConditions(fingerCount))
            continue;

        m_activePinchGestures << pinchGesture;
    }
}

bool GestureRecognizer::pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely)
{
    Q_UNUSED(angleDelta)
    Q_UNUSED(delta)

    const auto pinchDelta = -(m_previousPinchScale - scale);
    m_previousPinchScale = scale;

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
            const auto &gesture = *it;
            if (gesture->direction() != PinchDirection::Any && gesture->direction() != direction)
            {
                gesture->cancelled();
                it = m_activePinchGestures.erase(it);
                continue;
            }
            else if (gesture->update(pinchDelta))
            {
                endedPrematurely = true;
                return true;
            }

            it++;
        }
    }

    return !m_activePinchGestures.isEmpty();
}

void GestureRecognizer::pinchGestureCancelled()
{
    for (const auto &gesture : m_activePinchGestures)
        gesture->cancelled();

    m_activePinchGestures.clear();
    m_currentFingerCount = 0;
    m_previousPinchScale = 1;
}

bool GestureRecognizer::pinchGestureEnd()
{
    bool hadActiveGestures = !m_activePinchGestures.isEmpty();
    for (const auto &gesture : m_activePinchGestures)
        gesture->ended();

    m_activePinchGestures.clear();
    m_currentFingerCount = 0;
    m_previousPinchScale = 1;

    return hadActiveGestures;
}