#include "handler.h"

#include "libgestures/input.h"

#include "libgestures/yaml_convert.h"

namespace libgestures
{

static qreal s_holdDelta = 5; // 200 Hz
static uint32_t s_mousePointerAxisTimeout = 500;
static uint32_t s_touchpadPointerAxisTimeout = 100;
static uint32_t s_longPointerAxisTimeout = 2000;

GestureHandler::GestureHandler()
{
    m_pressTimer.setTimerType(Qt::PreciseTimer);
    connect(&m_pressTimer, &QTimer::timeout, this, [this] {
        pressGestureUpdate(s_holdDelta);
    });

    m_pointerAxisTimeoutTimer.setSingleShot(true);
    connect(&m_pointerAxisTimeoutTimer, &QTimer::timeout, this, [this] {
        gestureEnd(GestureType::Swipe | GestureType::Wheel);
    });

    m_mouseButtonTimer.setTimerType(Qt::TimerType::PreciseTimer);
    m_mouseButtonTimer.setSingleShot(true);

    m_mouseTimeoutTimer.setTimerType(Qt::TimerType::PreciseTimer);
    m_mouseTimeoutTimer.setSingleShot(true);

    m_mouseLongPointerAxisTimeoutTimer.setTimerType(Qt::TimerType::PreciseTimer);
    m_mouseLongPointerAxisTimeoutTimer.setSingleShot(true);
}

void GestureHandler::setDeviceType(const DeviceType &type)
{
    m_deviceType = type;
}

void GestureHandler::registerGesture(std::shared_ptr<Gesture> gesture)
{
    GestureType type;
    if (std::dynamic_pointer_cast<PressGesture>(gesture)) {
        type = GestureType::Press;
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

std::vector<Gesture *> GestureHandler::activatableGestures(const GestureType &type, const GestureBeginEvent &data)
{
    std::vector<Gesture *> gestures;
    for (const auto &gesture : m_gestures[type]) {
        if (!gesture->satisfiesBeginConditions(data)) {
            continue;
        }

        gestures.push_back(gesture.get());
    }
    return gestures;
}

bool GestureHandler::hasActiveGestures(const GestureType &type)
{
    return !m_activeGestures[type].empty();
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

void GestureHandler::pressGestureUpdate(const qreal &delta)
{
    for (const auto &gesture : m_activeGestures[GestureType::Press]) {
        if (!gesture->update(delta)) {
            return;
        }
    }
}

bool GestureHandler::pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &ended)
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
            gestureCancel(GestureType::Pinch);
        } else if (std::abs(1.0 - scale) >= 0.2) {
            m_pinchType = PinchType::Pinch;
            gestureCancel(GestureType::Rotate);
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
        auto &activeGestures = m_activeGestures[GestureType::Rotate];
        for (auto it = activeGestures.begin(); it != activeGestures.end();) {
            const auto gesture = dynamic_cast<RotateGesture *>(*it);
            if (!gesture->satisfiesUpdateConditions(m_speed, rotateDirection)) {
                gesture->cancel();
                it = activeGestures.erase(it);
                continue;
            }

            if (!gesture->update(angleDelta)) {
                ended = true;
                return true;
            }

            it++;
        }
    } else if (m_pinchType == PinchType::Pinch) {
        auto &activeGestures = m_activeGestures[GestureType::Pinch];
        for (auto it = activeGestures.begin(); it != activeGestures.end();) {
            const auto gesture = dynamic_cast<PinchGesture *>(*it);
            if (!gesture->satisfiesUpdateConditions(m_speed, pinchDirection)) {
                gesture->cancel();
                it = activeGestures.erase(it);
                continue;
            }

            if (!gesture->update(pinchDelta, QPointF())) {
                ended = true;
                return true;
            }

            it++;
        }
    }

    return !m_activeGestures[GestureType::Pinch].empty()
        || !m_gestures[GestureType::Rotate].empty();
}

bool GestureHandler::swipeGestureUpdate(const QPointF &delta, bool &ended)
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

    auto &activeGestures = m_activeGestures[GestureType::Swipe];
    for (auto it = activeGestures.begin(); it != activeGestures.end();) {
        const auto gesture = dynamic_cast<SwipeGesture *>(*it);
        if (!gesture->satisfiesUpdateConditions(m_speed, direction)) {
            gesture->cancel();
            it = activeGestures.erase(it);
            continue;
        }

        if (!gesture->update(swipeAxis == Axis::Vertical ? delta.y() : delta.x(), delta * m_deltaMultiplier)) {
            ended = true;
            return true;
        }

        it++;
    }

    return !activeGestures.empty();
}

bool GestureHandler::wheelGestureUpdate(const QPointF &delta)
{
    SwipeDirection direction = SwipeDirection::Left;
    if (delta.x() > 0) {
        direction = SwipeDirection::Right;
    } else if (delta.y() > 0) {
        direction = SwipeDirection::Down;
    } else if (delta.y() < 0) {
        direction = SwipeDirection::Up;
    }

    auto &activeGestures = m_activeGestures[GestureType::Wheel];
    for (auto it = activeGestures.begin(); it != activeGestures.end();) {
        const auto gesture = dynamic_cast<WheelGesture *>(*it);
        if (!gesture->satisfiesUpdateConditions(GestureSpeed::Any, direction)) {
            gesture->cancel();
            it = activeGestures.erase(it);
            continue;
        }

        if (!gesture->update(std::abs(delta.x()) > 0 ? delta.x() : delta.y())) {
            return true;
        }

        it++;
    }

    return !activeGestures.empty();
}

void GestureHandler::pointerMotion(const QPointF &delta)
{
    if (Input::implementation()->isSendingInput()) {
        return;
    }

    // If the user holds a button, activating a hold gesture, but then moves the mouse, and there are no swipe
    // gestures, press the blocked buttons to allow dragging/selection.
//    if ((gestureCancel(GestureType::Press) || m_mouseButtonTimer.isActive() || m_mouseTimeoutTimer.isActive())
//        && !hasActiveGestures(GestureType::Swipe)) {
//        pressBlockedMouseButtons();
//    }

    if (m_mouseButtonTimer.isActive() || m_mouseTimeoutTimer.isActive()) {
        gestureCancel(GestureType::All);

        m_mouseButtonTimer.stop();
        m_mouseTimeoutTimer.stop();
        if (!gestureBegin(GestureType::Swipe)) {
            pressBlockedMouseButtons();
        }
    }

    const auto hadActiveGestures = hasActiveGestures(GestureType::Swipe);
    bool _ = false;
    swipeGestureUpdate(delta, _);
    if (hadActiveGestures && !hasActiveGestures(GestureType::Swipe)) {
        // Swipe gesture cancelled due to wrong speed or direction
        pressBlockedMouseButtons();
    }
}

bool GestureHandler::pointerButton(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state)
{
    if (Input::implementation()->isSendingInput()) {
        return false;
    }

    if (state) {
        gestureCancel(GestureType::All);

        m_data = {};

        // This should be per-gesture instead of global, but it's good enough
        m_instantPress = false;
        for (const auto &gesture : activatableGestures(GestureType::Press, m_data)) {
            if (dynamic_cast<PressGesture *>(gesture)->instant()) {
                m_instantPress = true;
                break;
            }
        }

        disconnect(&m_mouseButtonTimer, nullptr, nullptr, nullptr);
        disconnect(&m_mouseTimeoutTimer, nullptr, nullptr, nullptr);
        connect(&m_mouseButtonTimer, &QTimer::timeout, this, [this] {
            const auto swipeTimeout = [this] {
                if (!hasActiveGestures(GestureType::Wheel) && !gestureBegin(GestureType::Press, m_data)) {
                    pressBlockedMouseButtons();
                }
            };

            if (m_instantPress) {
                swipeTimeout();
                return;
            }

            connect(&m_mouseTimeoutTimer, &QTimer::timeout, [swipeTimeout] {
                swipeTimeout();
            });
            m_mouseTimeoutTimer.start(200);
        });
        m_mouseButtonTimer.start(50);

        if (shouldBlockMouseButton(button, m_data)) {
            m_blockedMouseButtons.push_back(nativeButton);
            return true;
        }
    } else {
        auto hadGesture = gestureEnd(libgestures::GestureType::All)
            || m_mouseLongPointerAxisTimeoutTimer.isActive();

        // Prevent gesture skipping when clicking rapidly
        if (m_mouseButtonTimer.isActive() || m_mouseTimeoutTimer.isActive()) {
            m_mouseButtonTimer.stop();
            m_mouseTimeoutTimer.stop();

            if (m_instantPress) {
                gestureBegin(GestureType::Press, m_data);
                pressGestureUpdate(s_holdDelta);
                hadGesture = gestureEnd(GestureType::Press) || hadGesture;
            }
        }

        if (m_blockedMouseButtons.removeAll(nativeButton) && !hadGesture) {
            Input::implementation()->mouseButton(nativeButton, true);
            Input::implementation()->mouseButton(nativeButton, false);
        }
    }

    return false;
}

bool GestureHandler::pointerAxis(const QPointF &delta)
{
    const auto mouse = m_deviceType == DeviceType::Mouse;
    if (!m_pointerAxisTimeoutTimer.isActive()) {
        if (mouse) {
            gestureBegin(GestureType::Wheel);
        } else {
            gestureBegin(GestureType::Swipe, 2);
        }
    }
    m_pointerAxisTimeoutTimer.stop();
    m_pointerAxisTimeoutTimer.start(mouse ? s_mousePointerAxisTimeout : s_touchpadPointerAxisTimeout);

    bool _ = false;
    if (mouse ? wheelGestureUpdate(delta) : swipeGestureUpdate(delta, _)) {
        m_mouseLongPointerAxisTimeoutTimer.start(s_longPointerAxisTimeout);
        return true;
    }
    m_pointerAxisTimeoutTimer.stop();
//    m_mouseLongPointerAxisTimeoutTimer.stop();
    return false;
}

bool GestureHandler::shouldBlockMouseButton(const Qt::MouseButton &button, const GestureBeginEvent &event)
{
    for (const auto &[type, gestures] : m_gestures) {
        for (const auto &gesture : gestures) {
            const auto gestureModifiers = gesture->keyboardModifiers();
            const auto gestureButtons = gesture->mouseButtons();
            const auto gestureEdges = gesture->edges();
            if ((gestureModifiers && *gestureModifiers != event.keyboardModifiers)
                || (gestureEdges && !gestureEdges->contains(event.edges))) {
                continue;
            }
            if (gestureButtons && (*gestureButtons & button)) {
                return true;
            }
        }
    }
    return false;
}

void GestureHandler::pressBlockedMouseButtons()
{
    for (const auto &button : m_blockedMouseButtons) {
        Input::implementation()->mouseButton(button, true);
    }
    m_blockedMouseButtons.clear();
}

bool GestureHandler::gestureBegin(const GestureTypes &types, const uint8_t &fingers)
{
    return gestureBegin(types, GestureBeginEvent{
        .fingers = fingers
    });
}

bool GestureHandler::gestureBegin(const GestureTypes &types, const GestureBeginEvent &data)
{
    gestureCancel(GestureType::All);
    resetMembers();

    auto hasGestures = false;
    auto hasKeyboardModifiers = false;
    for (const auto &[type, _] : m_gestures) {
        if (!(types & type)) {
            continue;
        }

        auto &activeGestures = m_activeGestures[type];
        if (!activeGestures.empty()) {
            return true;
        }

        if (type == GestureType::Press) {
            m_pressTimer.start(s_holdDelta);
        }

        for (auto &gesture : activatableGestures(type, data)) {
            m_isDeterminingSpeed = m_isDeterminingSpeed || gesture->speed() != GestureSpeed::Any;
            hasKeyboardModifiers = hasKeyboardModifiers || (gesture->keyboardModifiers() && *gesture->keyboardModifiers() != Qt::KeyboardModifier::NoModifier);

            activeGestures.push_back(gesture);
        }

        hasGestures = hasGestures || !activeGestures.empty();
    }
    if (hasKeyboardModifiers) {
        Input::implementation()->keyboardClearModifiers();
    }

    return hasGestures;
}

bool GestureHandler::gestureEnd(const GestureTypes &types)
{
    return gestureEndOrCancel(types);
}

bool GestureHandler::gestureCancel(const GestureTypes &types)
{
    return gestureEndOrCancel(types, false);
}

bool GestureHandler::gestureEndOrCancel(const GestureTypes &types, const bool &end)
{
    m_mouseLongPointerAxisTimeoutTimer.stop();
    auto hadActiveGestures = false;
    for (auto &[type, gestures] : m_activeGestures) {
        if (!(types & type)) {
            continue;
        }

        if (type == GestureType::Press) {
            m_pressTimer.stop();
        }

        hadActiveGestures = hadActiveGestures || !gestures.empty();
        for (const auto &gesture : gestures) {
            if (end) {
                gesture->end();
                continue;
            }

            gesture->cancel();
        }
        gestures.clear();
    }
    return hadActiveGestures;
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