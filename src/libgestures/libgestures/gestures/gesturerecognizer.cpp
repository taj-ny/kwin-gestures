#include "gesturerecognizer.h"

namespace libgestures
{

#define TEMPLATES(TGesture)                                                                                                                      \
    template void GestureRecognizer::gestureBegin<TGesture>(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures); \
    template bool GestureRecognizer::gestureEnd<TGesture>(std::vector<std::shared_ptr<TGesture>> & activeGestures);                              \
    template void GestureRecognizer::gestureCancel<TGesture>(std::vector<std::shared_ptr<TGesture>> & activeGestures);
TEMPLATES(HoldGesture)
TEMPLATES(PinchGesture)
TEMPLATES(SwipeGesture)

void GestureRecognizer::registerGesture(std::shared_ptr<Gesture> gesture)
{
    auto removeBlock = [this](Gesture *gesture) {
        if (m_blockingGesture == gesture) {
            m_blockingGesture = nullptr;
        }
    };
    connect(gesture.get(), &Gesture::ended, this, [gesture, removeBlock]() {
        removeBlock(gesture.get());
    });
    connect(gesture.get(), &Gesture::cancelled, this, [gesture, removeBlock]() {
        removeBlock(gesture.get());
    });

    m_gestures.push_back(gesture);

}

void GestureRecognizer::unregisterGestures()
{
    m_gestures.clear();
}

void GestureRecognizer::setInputEventsToSample(const uint8_t &events)
{
    m_inputEventsToSample = events;
}

void GestureRecognizer::setSwipeFastThreshold(const qreal &threshold)
{
    m_swipeGestureFastThreshold = threshold;
}

void GestureRecognizer::setPinchInFastThreshold(const qreal &threshold)
{
    m_pinchInFastThreshold = threshold;
}

void GestureRecognizer::setPinchOutFastThreshold(const qreal &threshold)
{
    m_pinchOutFastThreshold = threshold;
}

void GestureRecognizer::setDeltaMultiplier(const qreal &deltaMultiplier)
{
    m_deltaMultiplier = deltaMultiplier;
}

void GestureRecognizer::holdGestureUpdate(const qreal &delta, bool &endedPrematurely)
{
    for (const auto &holdGesture : m_activeHoldGestures) {
        Q_EMIT holdGesture->updated(QPointF(delta, delta), QPointF(), endedPrematurely);
        if (endedPrematurely)
            return;
    }
}

bool GestureRecognizer::pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely)
{
    Q_UNUSED(angleDelta)
    Q_UNUSED(delta)

    const auto pinchDelta = -(m_previousPinchScale - scale);
    m_previousPinchScale = scale;

    // Determine the direction of the swipe
    PinchDirection direction = scale < 1 ? PinchDirection::In : PinchDirection::Out;

    if (m_isDeterminingSpeed) {
        if (m_sampledInputEvents++ != m_inputEventsToSample) {
            m_accumulatedAbsoluteSampledDelta += std::abs(pinchDelta);
            return true;
        }

        if ((m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= (scale < 1 ? m_pinchInFastThreshold : m_pinchOutFastThreshold))
            m_speed = GestureSpeed::Fast;
        else
            m_speed = GestureSpeed::Slow;

        m_isDeterminingSpeed = false;
    }

    for (auto it = m_activePinchGestures.begin(); it != m_activePinchGestures.end();) {
        const auto &gesture = *it;
        if (gesture->blocksOthers() && !m_blockingGesture) {
            m_blockingGesture = gesture.get();
        }

        if ((gesture->direction() != PinchDirection::Any && gesture->direction() != direction)
            || (gesture->speed() != GestureSpeed::Any && gesture->speed() != m_speed)) {
            gesture->cancelled();
            it = m_activePinchGestures.erase(it);
            continue;
        }

        if (m_blockingGesture && m_blockingGesture != gesture.get()) {
            it++;
            continue;
        }

        Q_EMIT gesture->updated(QPointF(pinchDelta, pinchDelta), QPointF(), endedPrematurely);
        if (endedPrematurely)
            return true;

        it++;
    }

    return !m_activePinchGestures.empty();
}

bool GestureRecognizer::swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely)
{
    m_currentSwipeDelta += delta;

    if (m_isDeterminingSpeed) {
        if (m_sampledInputEvents++ != m_inputEventsToSample) {
            m_accumulatedAbsoluteSampledDelta += std::abs(delta.x()) + std::abs(delta.y());
            return true;
        }

        if ((m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= m_swipeGestureFastThreshold)
            m_speed = GestureSpeed::Fast;
        else
            m_speed = GestureSpeed::Slow;

        m_isDeterminingSpeed = false;
    }

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

    for (auto it = m_activeSwipeGestures.begin(); it != m_activeSwipeGestures.end();) {
        const auto gesture = *it;
        if (gesture->blocksOthers() && !m_blockingGesture) {
            m_blockingGesture = gesture.get();
        }

        if ((!((gesture->direction() == SwipeDirection::LeftRight
                && (direction == SwipeDirection::Left || direction == SwipeDirection::Right))
               || (gesture->direction() == SwipeDirection::UpDown
                   && (direction == SwipeDirection::Up || direction == SwipeDirection::Down)))
             && (gesture->direction() != SwipeDirection::Any && gesture->direction() != direction))
            || (gesture->speed() != GestureSpeed::Any && gesture->speed() != m_speed)) {
            gesture->cancelled();
            it = m_activeSwipeGestures.erase(it);
            continue;
        }

        if (m_blockingGesture && m_blockingGesture != gesture.get()) {
            it++;
            continue;
        }

        Q_EMIT gesture->updated(delta, delta * m_deltaMultiplier, endedPrematurely);
        if (endedPrematurely)
            return true;

        it++;
    }

    return !m_activeSwipeGestures.empty();
}

void GestureRecognizer::holdGestureBegin(const uint8_t &fingerCount)
{
    gestureBegin(fingerCount, m_activeHoldGestures);
}

void GestureRecognizer::holdGestureCancel()
{
    gestureCancel(m_activeHoldGestures);
}

bool GestureRecognizer::holdGestureEnd()
{
    return gestureEnd(m_activeHoldGestures);
}

void GestureRecognizer::swipeGestureBegin(const uint8_t &fingerCount)
{
    gestureBegin(fingerCount, m_activeSwipeGestures);
}

void GestureRecognizer::swipeGestureCancel()
{
    gestureCancel(m_activeSwipeGestures);
}

bool GestureRecognizer::swipeGestureEnd()
{
    return gestureEnd(m_activeSwipeGestures);
}

void GestureRecognizer::pinchGestureBegin(const uint8_t &fingerCount)
{
    gestureBegin(fingerCount, m_activePinchGestures);
}

void GestureRecognizer::pinchGestureCancel()
{
    gestureCancel(m_activePinchGestures);
}

bool GestureRecognizer::pinchGestureEnd()
{
    return gestureEnd(m_activePinchGestures);
}

template<class TGesture>
void GestureRecognizer::gestureBegin(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    if (!activeGestures.empty())
        return;

    for (const std::shared_ptr<Gesture> &gesture : m_gestures) {
        std::shared_ptr<TGesture> castedGesture = std::dynamic_pointer_cast<TGesture>(gesture);
        if (!castedGesture || !gesture->satisfiesConditions(fingerCount))
            continue;

        if (castedGesture->speed() != GestureSpeed::Any)
            m_isDeterminingSpeed = true;

        activeGestures.push_back(castedGesture);
    }
}

template<class TGesture>
bool GestureRecognizer::gestureEnd(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    bool hadActiveGestures = !activeGestures.empty();
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->ended();
    activeGestures.clear();

    resetMembers();

    return hadActiveGestures;
}

template<class TGesture>
void GestureRecognizer::gestureCancel(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->cancelled();
    activeGestures.clear();

    resetMembers();
}

void GestureRecognizer::resetMembers()
{
    m_accumulatedAbsoluteSampledDelta = 0;
    m_sampledInputEvents = 0;
    m_isDeterminingSpeed = false;
    m_previousPinchScale = 1;
    m_currentSwipeAxis = Axis::None;
    m_currentSwipeDelta = QPointF();
    m_speed = GestureSpeed::Any;
}

}