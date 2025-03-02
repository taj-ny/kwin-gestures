#include "gesturerecognizer.h"

#include "libgestures/input.h"

namespace libgestures
{

#define TEMPLATES(TGesture)                                                                                                                      \
    template bool GestureRecognizer::gestureBegin<TGesture>(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures); \
    template bool GestureRecognizer::gestureEnd<TGesture>(std::vector<std::shared_ptr<TGesture>> & activeGestures);                              \
    template void GestureRecognizer::gestureCancel<TGesture>(std::vector<std::shared_ptr<TGesture>> & activeGestures);
TEMPLATES(HoldGesture)
TEMPLATES(PinchGesture)
TEMPLATES(SwipeGesture)

void GestureRecognizer::registerGesture(std::shared_ptr<Gesture> gesture)
{
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

void GestureRecognizer::setRotateFastThreshold(const qreal &threshold)
{
    m_rotateFastThreshold = threshold;
}

void GestureRecognizer::setDeltaMultiplier(const qreal &deltaMultiplier)
{
    m_deltaMultiplier = deltaMultiplier;
}

void GestureRecognizer::holdGestureUpdate(const qreal &delta, bool &endedPrematurely)
{
    for (const auto &holdGesture : m_activeHoldGestures) {
        Q_EMIT holdGesture->updated(delta, QPointF(), endedPrematurely);
        if (endedPrematurely)
            return;
    }
}

bool GestureRecognizer::pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely)
{
    Q_UNUSED(delta)

    const auto pinchDelta = -(m_previousPinchScale - scale);
    m_previousPinchScale = scale;
    PinchDirection pinchDirection = scale < 1 ? PinchDirection::In : PinchDirection::Out;

    RotateDirection rotateDirection = angleDelta > 0 ? RotateDirection::Clockwise : RotateDirection::Counterclockwise;
    m_accumulatedRotateDelta += std::abs(angleDelta);

    if (m_pinchType == PinchType::Unknown) {
        if (m_accumulatedRotateDelta >= 10) {
            m_pinchType = PinchType::Rotate;
            gestureCancel(m_activePinchGestures);
        } else if (std::abs(1.0 - scale) >= 0.2) {
            m_pinchType = PinchType::Pinch;
            gestureCancel(m_activeRotateGestures);
        } else {
            return true;
        }
    }

    if (m_isDeterminingSpeed) {
        if (m_sampledInputEvents++ != m_inputEventsToSample) {
            m_accumulatedAbsoluteSampledDelta += std::abs(m_pinchType == PinchType::Rotate ? angleDelta : pinchDelta);
            return true;
        }

        const auto speedThreshold = m_pinchType == PinchType::Rotate
            ? m_rotateFastThreshold
            : (scale < 1 ? m_pinchInFastThreshold : m_pinchOutFastThreshold);
        if ((m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= speedThreshold)
            m_speed = GestureSpeed::Fast;
        else
            m_speed = GestureSpeed::Slow;

        m_isDeterminingSpeed = false;
    }

    if (m_pinchType == PinchType::Rotate) {
        for (auto it = m_activeRotateGestures.begin(); it != m_activeRotateGestures.end();) {
            const auto &gesture = *it;
            if (!gesture->satisfiesUpdateConditions(m_speed, rotateDirection)) {
                gesture->cancelled();
                it = m_activeRotateGestures.erase(it);
                continue;
            }

            Q_EMIT gesture->updated(angleDelta, QPointF(), endedPrematurely);
            if (endedPrematurely) {
                return true;
            }

            it++;
        }
    } else if (m_pinchType == PinchType::Pinch) {
        for (auto it = m_activePinchGestures.begin(); it != m_activePinchGestures.end();) {
            const auto &gesture = *it;
            if (!gesture->satisfiesUpdateConditions(m_speed, pinchDirection)) {
                gesture->cancelled();
                it = m_activePinchGestures.erase(it);
                continue;
            }

            Q_EMIT gesture->updated(pinchDelta, QPointF(), endedPrematurely);
            if (endedPrematurely)
                return true;

            it++;
        }
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
        if (!gesture->satisfiesUpdateConditions(m_speed, direction)) {
            gesture->cancelled();
            it = m_activeSwipeGestures.erase(it);
            continue;
        }

        Q_EMIT gesture->updated(swipeAxis == Axis::Vertical ? delta.y() : delta.x(), delta * m_deltaMultiplier, endedPrematurely);
        if (endedPrematurely)
            return true;

        it++;
    }

    return !m_activeSwipeGestures.empty();
}

bool GestureRecognizer::holdGestureBegin(const uint8_t &fingerCount)
{
    resetMembers();
    return gestureBegin(fingerCount, m_activeHoldGestures);
}

void GestureRecognizer::holdGestureCancel()
{
    gestureCancel(m_activeHoldGestures);
}

bool GestureRecognizer::holdGestureEnd()
{
    return gestureEnd(m_activeHoldGestures);
}

bool GestureRecognizer::swipeGestureBegin(const uint8_t &fingerCount)
{
    resetMembers();
    return gestureBegin(fingerCount, m_activeSwipeGestures);
}

void GestureRecognizer::swipeGestureCancel()
{
    gestureCancel(m_activeSwipeGestures);
}

bool GestureRecognizer::swipeGestureEnd()
{
    return gestureEnd(m_activeSwipeGestures);
}

bool GestureRecognizer::pinchGestureBegin(const uint8_t &fingerCount)
{
    resetMembers();
    return gestureBegin(fingerCount, m_activePinchGestures)
        || gestureBegin(fingerCount, m_activeRotateGestures);
}

void GestureRecognizer::pinchGestureCancel()
{
    gestureCancel(m_activePinchGestures);
    gestureCancel(m_activeRotateGestures);
}

bool GestureRecognizer::pinchGestureEnd()
{
    return gestureEnd(m_activePinchGestures) || gestureEnd(m_activeRotateGestures);
}

template<class TGesture>
bool GestureRecognizer::gestureBegin(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    if (!activeGestures.empty()) {
        return true;
    }

    auto hasModifiers = false;
    for (const std::shared_ptr<Gesture> &gesture : m_gestures) {
        std::shared_ptr<TGesture> castedGesture = std::dynamic_pointer_cast<TGesture>(gesture);
        if (!castedGesture || !gesture->satisfiesBeginConditions(fingerCount))
            continue;

        if (castedGesture->speed() != GestureSpeed::Any) {
            m_isDeterminingSpeed = true;
        }

        if (castedGesture->keyboardModifiers() && *castedGesture->keyboardModifiers() != Qt::KeyboardModifier::NoModifier) {
            hasModifiers = true;
        }

        activeGestures.push_back(castedGesture);
    }

    if (hasModifiers) {
        Input::implementation()->keyboardClearModifiers();
    }

    return !activeGestures.empty();
}

template<class TGesture>
bool GestureRecognizer::gestureEnd(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    bool hadActiveGestures = !activeGestures.empty();
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->ended();
    activeGestures.clear();

    return hadActiveGestures;
}

template<class TGesture>
void GestureRecognizer::gestureCancel(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->cancelled();
    activeGestures.clear();
}

void GestureRecognizer::resetMembers()
{
    m_accumulatedAbsoluteSampledDelta = 0;
    m_sampledInputEvents = 0;
    m_pinchType = PinchType::Unknown;
    m_accumulatedRotateDelta = 0;
    m_isDeterminingSpeed = false;
    m_previousPinchScale = 1;
    m_currentSwipeAxis = Axis::None;
    m_currentSwipeDelta = QPointF();
    m_speed = GestureSpeed::Any;
}

}