#include "gesturerecognizer.h"

namespace libgestures
{

#define TEMPLATES(TGesture) \
template void GestureRecognizer::gestureBegin<TGesture>(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures); \
template bool GestureRecognizer::gestureEnd<TGesture>(std::vector<std::shared_ptr<TGesture>> &activeGestures); \
template void GestureRecognizer::gestureCancel<TGesture>(std::vector<std::shared_ptr<TGesture>> &activeGestures);
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

void GestureRecognizer::setWidthToHeightRatio(const qreal &ratio)
{
    m_widthToHeightRatio = ratio;
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

void GestureRecognizer::holdGestureUpdate(const qreal &delta, bool &endedPrematurely)
{
    for (const auto &holdGesture : m_activeHoldGestures)
    {
        Q_EMIT holdGesture->updated(delta, endedPrematurely);
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

    if (m_isDeterminingSpeed)
    {
        if (m_sampledInputEvents++ != m_inputEventsToSample)
        {
            m_accumulatedAbsoluteSampledDelta += std::abs(pinchDelta);
            return true;
        }

        if ((m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= (scale < 1 ? m_pinchInFastThreshold : m_pinchOutFastThreshold))
            m_speed = GestureSpeed::Fast;
        else
            m_speed = GestureSpeed::Slow;

        m_isDeterminingSpeed = false;
    }

    for (auto it = m_activePinchGestures.begin(); it != m_activePinchGestures.end();)
    {
        const auto &gesture = *it;
        if ((gesture->direction() != PinchDirection::Any && gesture->direction() != direction)
            || (gesture->speed() != GestureSpeed::Any && gesture->speed() != m_speed))
        {
            gesture->cancelled();
            it = m_activePinchGestures.erase(it);
            continue;
        }

        Q_EMIT gesture->updated(pinchDelta, endedPrematurely);
        if (endedPrematurely)
            return true;

        it++;
    }

    return !m_activePinchGestures.empty();
}

bool GestureRecognizer::swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely)
{
    m_currentSwipeDelta += delta;

    if (m_isDeterminingSpeed)
    {
        if (m_sampledInputEvents++ != m_inputEventsToSample)
        {
            m_accumulatedAbsoluteSampledDelta += std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2));
            m_currentSwipeDelta = {};
            return true;
        }

        if ((m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= m_swipeGestureFastThreshold)
            m_speed = GestureSpeed::Fast;
        else
            m_speed = GestureSpeed::Slow;

        m_isDeterminingSpeed = false;
    }

    SwipeDirection validDirection = SwipeDirection::None;
    SwipeDirection direction = SwipeDirection::None;

    if (m_validSwipeDirection == SwipeDirection::None) {
        if (isSwipeDiagonal(m_currentSwipeDelta)) {
            if ((m_currentSwipeDelta.x() > 0 && m_currentSwipeDelta.y() < 0)
                || (m_currentSwipeDelta.x() < 0 && m_currentSwipeDelta.y() > 0)) {
                validDirection = SwipeDirection::LeftDownRightUp;
            } else if ((m_currentSwipeDelta.x() > 0 && m_currentSwipeDelta.y() > 0)
                || (m_currentSwipeDelta.x() < 0 && m_currentSwipeDelta.y() < 0)) {
                validDirection = SwipeDirection::LeftUpRightDown;
            }
        } else if (std::abs(m_currentSwipeDelta.x()) > std::abs(m_currentSwipeDelta.y())) {
            validDirection = SwipeDirection::LeftRight;
        } else {
            validDirection = SwipeDirection::UpDown;
        }

        if (std::abs(m_currentSwipeDelta.x()) >= 5 || std::abs(m_currentSwipeDelta.y()) >= 5) {
            // only lock in a direction if the delta is big enough
            // to prevent accidentally choosing the wrong direction
            m_validSwipeDirection = validDirection;
        }
    } else {
        validDirection = m_validSwipeDirection;
    }

    qreal deltaSingle = 0;
    if (validDirection == SwipeDirection::LeftRight) {
        direction = delta.x() < 0 ? SwipeDirection::Left : SwipeDirection::Right;
        deltaSingle = delta.x();
    } else if (validDirection == SwipeDirection::UpDown) {
        direction = delta.y() < 0 ? SwipeDirection::Up : SwipeDirection::Down;
        deltaSingle = delta.y();
    } else if (validDirection == SwipeDirection::LeftDownRightUp) {
        direction = delta.x() > 0 && delta.y() < 0 ? SwipeDirection::RightUp : SwipeDirection::LeftDown;
        deltaSingle = std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2));
    } else if (validDirection == SwipeDirection::LeftUpRightDown) {
        direction = delta.x() < 0 && delta.y() < 0 ? SwipeDirection::LeftUp : SwipeDirection::RightDown;
        deltaSingle = std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2));
    }

    for (auto it = m_activeSwipeGestures.begin(); it != m_activeSwipeGestures.end();)
    {
        const auto gesture = *it;

        if ((!((gesture->direction() == SwipeDirection::LeftRight
               && (direction == SwipeDirection::Left || direction == SwipeDirection::Right))
              || (gesture->direction() == SwipeDirection::UpDown
                  && (direction == SwipeDirection::Up || direction == SwipeDirection::Down))
              || (gesture->direction() == SwipeDirection::LeftDownRightUp
                  && (direction == SwipeDirection::LeftDown || direction == SwipeDirection::RightUp))
              || (gesture->direction() == SwipeDirection::LeftUpRightDown
                  && (direction == SwipeDirection::LeftUp || direction == SwipeDirection::RightDown)))
            && gesture->direction() != direction)
            || (gesture->speed() != GestureSpeed::Any && gesture->speed() != m_speed))
        {
            gesture->cancelled();
            it = m_activeSwipeGestures.erase(it);
            continue;
        }

        Q_EMIT gesture->updated(deltaSingle, endedPrematurely);
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

template <class TGesture>
void GestureRecognizer::gestureBegin(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    if (!activeGestures.empty())
        return;

    for (const std::shared_ptr<Gesture> &gesture : m_gestures)
    {
        std::shared_ptr<TGesture> castedGesture = std::dynamic_pointer_cast<TGesture>(gesture);
        if (!castedGesture || !gesture->satisfiesConditions(fingerCount))
            continue;

        if (castedGesture->speed() != GestureSpeed::Any)
            m_isDeterminingSpeed = true;

        activeGestures.push_back(castedGesture);
    }
}

template <class TGesture>
bool GestureRecognizer::gestureEnd(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    bool hadActiveGestures = !activeGestures.empty();
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->ended();
    activeGestures.clear();

    resetMembers();

    return hadActiveGestures;
}

template <class TGesture>
void GestureRecognizer::gestureCancel(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->cancelled();
    activeGestures.clear();

    resetMembers();
}

bool GestureRecognizer::isSwipeDiagonal(const QPointF &delta) const
{
    constexpr auto min = 35 * (M_PI / 180);
    constexpr auto max = 70 * (M_PI / 180);
    const auto angle = atan2(std::abs(delta.y() * m_widthToHeightRatio), std::abs(delta.x()));
    return angle >= min && angle <= max;
}

void GestureRecognizer::resetMembers()
{
    m_accumulatedAbsoluteSampledDelta = 0;
    m_sampledInputEvents = 0;
    m_isDeterminingSpeed = false;
    m_previousPinchScale = 1;
    m_validSwipeDirection = SwipeDirection::None;
    m_currentSwipeDelta = QPointF();
    m_speed = GestureSpeed::Any;
}

}