#include "handler.h"

#include "libgestures/input.h"

namespace libgestures
{

#define TEMPLATES(TGesture)                                                                                                                      \
    template bool GestureHandler::gestureBegin<TGesture>(const uint8_t &fingerCount, const GestureType &type, std::vector<std::shared_ptr<TGesture>> &activeGestures); \
    template bool GestureHandler::gestureEnd<TGesture>(std::vector<std::shared_ptr<TGesture>> &activeGestures);                              \
    template void GestureHandler::gestureCancel<TGesture>(std::vector<std::shared_ptr<TGesture>> &activeGestures);
TEMPLATES(HoldGesture)
TEMPLATES(PinchGesture)
TEMPLATES(SwipeGesture)
TEMPLATES(WheelGesture)

static qreal s_holdDelta = 5; // 200 Hz
static uint32_t s_mousePointerAxisTimeout = 500;
static uint32_t s_touchpadPointerAxisTimeout = 100;

GestureHandler::GestureHandler()
{
    m_holdTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_holdTimer, &QTimer::timeout, this, [this]() {
        bool _ = false;
        holdGestureUpdate(s_holdDelta, _);
    });

    m_pointerAxisTimer.setSingleShot(true);
    connect(&m_pointerAxisTimer, &QTimer::timeout, this, [this]() {
        gestureEnd(GestureType::Swipe);
        gestureEnd(GestureType::Wheel);
    });
}

void GestureHandler::setDeviceType(const DeviceType &type)
{
    m_deviceType = type;
}

void GestureHandler::registerGesture(std::shared_ptr<Gesture> gesture)
{
    GestureType type;
    if (std::dynamic_pointer_cast<HoldGesture>(gesture)) {
        type = GestureType::Hold;
    } else if (std::dynamic_pointer_cast<PinchGesture>(gesture)) {
        type = GestureType::Pinch;
    } else if (std::dynamic_pointer_cast<RotateGesture>(gesture)) {
        type = GestureType::Rotate;
    } else if (std::dynamic_pointer_cast<WheelGesture>(gesture)) {
        type = GestureType::Wheel;
    } else if (std::dynamic_pointer_cast<SwipeGesture>(gesture)) {
        type = GestureType::Swipe;
    } else {
        return;
    }

    m_gestures[type].push_back(gesture);
}

void GestureHandler::unregisterGestures()
{
    m_gestures.clear();
}

void GestureHandler::setInputEventsToSample(const uint8_t &events)
{
    m_inputEventsToSample = events;
}

void GestureHandler::setSwipeFastThreshold(const qreal &threshold)
{
    m_swipeGestureFastThreshold = threshold;
}

void GestureHandler::setPinchInFastThreshold(const qreal &threshold)
{
    m_pinchInFastThreshold = threshold;
}

void GestureHandler::setPinchOutFastThreshold(const qreal &threshold)
{
    m_pinchOutFastThreshold = threshold;
}

void GestureHandler::setRotateFastThreshold(const qreal &threshold)
{
    m_rotateFastThreshold = threshold;
}

void GestureHandler::setDeltaMultiplier(const qreal &deltaMultiplier)
{
    m_deltaMultiplier = deltaMultiplier;
}

void GestureHandler::holdGestureUpdate(const qreal &delta, bool &endedPrematurely)
{
    for (const auto &holdGesture : m_activeHoldGestures) {
        Q_EMIT holdGesture->updated(delta, QPointF(), endedPrematurely);
        if (endedPrematurely)
            return;
    }
}

bool GestureHandler::pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely)
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

bool GestureHandler::swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely)
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

bool GestureHandler::wheelGestureUpdate(const QPointF &delta)
{
    bool _ = false;
    return swipeGestureUpdate(delta, _);
}

void GestureHandler::pointerMotion(const QPointF &delta)
{
    if (m_pointerAxisTimer.isActive() || libgestures::Input::implementation()->isSendingInput()) {
        return;
    }

    bool _ = false;
    swipeGestureUpdate(delta, _);
}

bool GestureHandler::pointerButton(const bool &state)
{
    if (libgestures::Input::implementation()->isSendingInput()) {
        return false;
    }

    if (state) {
        gestureCancel(libgestures::GestureType::Hold);
        gestureCancel(libgestures::GestureType::Swipe);
        gestureCancel(libgestures::GestureType::Wheel);
        if (gestureBegin(libgestures::GestureType::Hold, 1)
            || gestureBegin(libgestures::GestureType::Swipe, 1)
            || hasActivatableGestures(libgestures::GestureType::Wheel)) {
            return true;
        }
    } else {
        gestureEnd(libgestures::GestureType::Hold);
        gestureEnd(libgestures::GestureType::Swipe);
        gestureEnd(libgestures::GestureType::Wheel);
    }

    return false;
}

bool GestureHandler::pointerAxis(const QPointF &delta)
{
    const auto mouse = m_deviceType == DeviceType::Mouse;
    if (!m_pointerAxisTimer.isActive()) {
        if (mouse) {
            gestureBegin(GestureType::Wheel);
        } else {
            gestureBegin(GestureType::Swipe, 2);
        }
    }
    m_pointerAxisTimer.stop();
    m_pointerAxisTimer.start(mouse ? s_mousePointerAxisTimeout : s_touchpadPointerAxisTimeout);

    bool _ = false;
    if (mouse ? wheelGestureUpdate(delta) : swipeGestureUpdate(delta, _)) {
        return true;
    }
    return false;
}

bool GestureHandler::hasActivatableGestures(const GestureType &type, const uint8_t &fingerCount)
{
    const auto gestures = m_gestures[type];
    return std::find_if(gestures.begin(), gestures.end(), [&fingerCount](const auto &gesture) {
        return gesture->satisfiesBeginConditions(fingerCount);
    }) != gestures.end();
}

bool GestureHandler::gestureBegin(const GestureType &type, const uint8_t &fingerCount)
{
    resetMembers();
    switch (type) {
        case GestureType::Hold:
            m_holdTimer.start(s_holdDelta);
            return gestureBegin(fingerCount, type, m_activeHoldGestures);
        case GestureType::Pinch:
        case GestureType::Rotate:
            return gestureBegin(fingerCount, GestureType::Pinch, m_activePinchGestures)
                   || gestureBegin(fingerCount, GestureType::Rotate, m_activeRotateGestures);
        case GestureType::Swipe:
            return gestureBegin(fingerCount, GestureType::Swipe, m_activeSwipeGestures);
        case GestureType::Wheel:
            return gestureBegin(0, GestureType::Wheel, m_activeSwipeGestures);
    }

    return false;
}

bool GestureHandler::gestureEnd(const GestureType &type)
{
    switch (type) {
        case GestureType::Hold:
            m_holdTimer.stop();
            return gestureEnd(m_activeHoldGestures);
        case GestureType::Pinch:
        case GestureType::Rotate:
            return gestureEnd(m_activePinchGestures) || gestureEnd(m_activeRotateGestures);
        case GestureType::Swipe:
        case GestureType::Wheel:
            return gestureEnd(m_activeSwipeGestures);
    }

    return false;
}

void GestureHandler::gestureCancel(const GestureType &type)
{
    switch (type) {
        case GestureType::Hold:
            m_holdTimer.stop();
            gestureCancel(m_activeHoldGestures);
        case GestureType::Pinch:
        case GestureType::Rotate:
            gestureCancel(m_activePinchGestures);
            gestureCancel(m_activeRotateGestures);
        case GestureType::Swipe:
        case GestureType::Wheel:
            gestureCancel(m_activeSwipeGestures);
    }
}

template<class TGesture>
bool GestureHandler::gestureBegin(const uint8_t &fingerCount, const GestureType &type, std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    if (!activeGestures.empty()) {
        return true;
    }

    auto hasModifiers = false;
    for (const std::shared_ptr<Gesture> &gesture : m_gestures[type]) {
        std::shared_ptr<TGesture> castedGesture = std::dynamic_pointer_cast<TGesture>(gesture);
        if (!castedGesture || !gesture->satisfiesBeginConditions(fingerCount)) {
            continue;
        }

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
bool GestureHandler::gestureEnd(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    bool hadActiveGestures = !activeGestures.empty();
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->ended();
    activeGestures.clear();

    return hadActiveGestures;
}

template<class TGesture>
void GestureHandler::gestureCancel(std::vector<std::shared_ptr<TGesture>> &activeGestures)
{
    for (const auto &gesture : activeGestures)
        Q_EMIT gesture->cancelled();
    activeGestures.clear();
}

void GestureHandler::resetMembers()
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