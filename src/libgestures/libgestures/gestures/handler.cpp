#include "handler.h"

#include "libgestures/logging.h"
#include "libgestures/input.h"

#include "libgestures/yaml_convert.h"

namespace libgestures
{

static qreal s_holdDelta = 5; // 200 Hz
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
        gestureEnd(GestureType::Stroke | GestureType::Swipe | GestureType::Wheel);
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
    m_gestures.push_back(gesture);
}

std::vector<Gesture *> GestureHandler::gestures(const GestureTypes &types, const std::optional<GestureBeginEvent> &data)
{
    std::vector<Gesture *> result;
    for (auto &gesture : m_gestures) {
        if (!(types & gesture->type()) || (data && !gesture->satisfiesBeginConditions(*data))) {
            continue;
        }

        result.push_back(gesture.get());
    }
    return result;
}

std::vector<Gesture *> GestureHandler::activeGestures(const GestureTypes &types)
{
    std::vector<Gesture *> result;
    for (auto &gesture : m_activeGestures) {
        if (!(types & gesture->type())) {
            continue;
        }

        result.push_back(gesture);
    }
    return result;
}

bool GestureHandler::updateGesture(const GestureType &type, GestureUpdateEvent event)
{
    return updateGesture({
        { type, event }
    });
}

bool GestureHandler::updateGesture(const std::map<GestureType, GestureUpdateEvent> &events)
{
    GestureTypes types(0);
    for (const auto &[type, _] : events) {
        types |= type;
    }
    qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "GesturesUpdating(types: " << types << ")";

    auto hasGestures = false;
    for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();) {
        auto gesture = *it;
        const auto &type = gesture->type();
        if (!(types & type)) {
            it++;
            continue;
        }

        const auto &event = events.at(type);
        if (!gesture->satisfiesUpdateConditions(m_speed, event.direction)) {
            gesture->cancel();
            it = m_activeGestures.erase(it);
            continue;
        }

        if (!gesture->update(event.delta, event.deltaPointMultiplied)) {
            *event.ended = true;
            return true;
        }
        if (m_activeGestures.size() > 1 && gesture->shouldCancelOtherGestures()) {
            gestureCancel(gesture);
            break;
        }

        it++;
        hasGestures = hasGestures || !m_activeGestures.empty();
    }
    return hasGestures;
}

bool GestureHandler::hasActiveGestures(const GestureTypes &types)
{
    for (const auto &gesture : m_activeGestures) {
        if (types & gesture->type()) {
            return true;
        }
    }
    return false;
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
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Event<Press>(delta: " << delta << ")";
    auto start = std::chrono::steady_clock::now();

    bool _ = false;
    const auto hasGestures = updateGesture(GestureType::Press, {
        .delta = delta,
        .ended = &_
    });

    auto end = std::chrono::steady_clock::now();
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "EventProcessed<Press>(hasGestures: " << hasGestures << ", time: " << since(start, end) << ")";
}

bool GestureHandler::pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &ended)
{
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Event<Pinch>(scale: " << scale << ", angleDelta: " << angleDelta << ", delta: " << delta << ")";
    auto start = std::chrono::steady_clock::now();

    const auto pinchDelta = -(m_previousPinchScale - scale);
    m_previousPinchScale = scale;

    PinchDirection pinchDirection = scale < 1 ? PinchDirection::In : PinchDirection::Out;

    RotateDirection rotateDirection = angleDelta > 0 ? RotateDirection::Clockwise : RotateDirection::Counterclockwise;
    m_accumulatedRotateDelta += std::abs(angleDelta);

    if (m_pinchType == PinchType::Unknown) {
        if (m_accumulatedRotateDelta >= 10) {
            qCDebug(LIBGESTURES_GESTURE_HANDLER) << "PinchTypeDetermined(type: rotate)";
            m_pinchType = PinchType::Rotate;
            gestureCancel(GestureType::Pinch);
        } else if (std::abs(1.0 - scale) >= 0.2) {
            qCDebug(LIBGESTURES_GESTURE_HANDLER) << "PinchTypeDetermined(type: pinch)";
            m_pinchType = PinchType::Pinch;
            gestureCancel(GestureType::Rotate);
        } else {
            auto end = std::chrono::steady_clock::now();
            qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace().noquote() << "EventProcessed<Pinch>(status: DeterminingType, time: " << since(start, end) << ")";
            return true;
        }
    }

    const auto speedThreshold = m_pinchType == PinchType::Rotate
        ? m_rotateFastThreshold
        : (scale < 1 ? m_pinchInFastThreshold : m_pinchOutFastThreshold);
    if (!determineSpeed(std::abs(m_pinchType == PinchType::Rotate ? angleDelta : pinchDelta), speedThreshold)) {
        auto end = std::chrono::steady_clock::now();
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace().noquote() << "EventProcessed<Pinch>(status: DeterminingSpeed, time: " << since(start, end) << ")";
        return true;
    }

    auto hasGestures = false;
    if (m_pinchType == PinchType::Rotate) {
        hasGestures = updateGesture(GestureType::Rotate, {
            .delta = angleDelta,
            .direction = static_cast<uint32_t>(rotateDirection),
            .ended = &ended
        });
    } else if (m_pinchType == PinchType::Pinch) {
        hasGestures = updateGesture(GestureType::Pinch, {
            .delta = pinchDelta,
            .direction = static_cast<uint32_t>(pinchDirection),
            .ended = &ended
        });
    }

    auto end = std::chrono::steady_clock::now();
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace().noquote() << "EventProcessed<Pinch>(hasGestures: " << hasGestures << ", time: " << since(start, end) << ")";
    return false;
}

bool GestureHandler::determineSpeed(const qreal &delta, const qreal &fastThreshold)
{
    if (!m_isDeterminingSpeed) {
        return true;
    }

    if (m_sampledInputEvents++ != m_inputEventsToSample) {
        m_accumulatedAbsoluteSampledDelta += delta;
        qCDebug(LIBGESTURES_GESTURE_HANDLER)
            << qPrintable(QString("SpeedDetermining(event: %1/%2, delta: %3/%4)")
                .arg(QString::number(m_sampledInputEvents), QString::number(m_inputEventsToSample), QString::number(m_accumulatedAbsoluteSampledDelta), QString::number(fastThreshold)));
        return false;
    }

    m_isDeterminingSpeed = false;
    m_speed = (m_accumulatedAbsoluteSampledDelta / m_inputEventsToSample) >= fastThreshold
        ? GestureSpeed::Fast
        : GestureSpeed::Slow;
    qCDebug(LIBGESTURES_GESTURE_HANDLER) << qPrintable(QString("SpeedDetermined(speed: %1)").arg(m_speed == GestureSpeed::Fast ? "fast" : "slow"));
    return true;
}

bool GestureHandler::touchMotion(const QPointF &delta, bool &ended)
{
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Event<Motion>(delta: " << delta << ")";
    auto start = std::chrono::steady_clock::now();

    if (!hasActiveGestures(GestureType::Stroke | GestureType::Swipe)) {
        auto end = std::chrono::steady_clock::now();
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace().noquote() << "EventProcessed<Motion>(status: NoGestures, time: " << since(start, end) << ")";
        return false;
    }

    const auto hasStroke = hasActiveGestures(GestureType::Stroke);
//    if (hasStroke) {
        m_stroke.push_back(delta);
//    } else {
        m_currentSwipeDelta += delta;
//    }

    const auto deltaTotal = std::sqrt(std::pow(delta.x(), 2) + std::pow(delta.y(), 2));
    if (!determineSpeed(deltaTotal, m_swipeGestureFastThreshold)) {
        auto end = std::chrono::steady_clock::now();
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace().noquote() << "EventProcessed<Motion>(status: DeterminingSpeed, time: " << since(start, end) << ")";
        return true;
    }

    std::map<GestureType, GestureUpdateEvent> events;

    bool filter = false;
    if (hasActiveGestures(GestureType::Swipe)) {
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

        events[GestureType::Swipe] = GestureUpdateEvent{
            .delta = swipeAxis == Axis::Vertical ? delta.y() : delta.x(),
            .direction = static_cast<uint32_t>(direction),
            .ended = &ended,
            .deltaPointMultiplied = delta * m_deltaMultiplier
        };
    }

    if (hasStroke) {
        events[GestureType::Stroke] = GestureUpdateEvent{
            .delta = deltaTotal,
            .ended = &ended
        };
    }

    const auto ret = updateGesture(events);
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "EventProcessed<Motion>(hasGestures: " << ret << ", time: " << since(start) << ")";
    return ret;
}

bool GestureHandler::wheelGestureUpdate(const QPointF &delta)
{
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Event<Wheel>(delta: " << delta << ")";
    auto start = std::chrono::steady_clock::now();

    if (!hasActiveGestures(GestureType::Wheel)) {
        auto end = std::chrono::steady_clock::now();
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "EventProcessed<Wheel>(status: NoGestures, time: " << since(start, end) << ")";
        return false;
    }

    SwipeDirection direction = SwipeDirection::Left;
    if (delta.x() > 0) {
        direction = SwipeDirection::Right;
    } else if (delta.y() > 0) {
        direction = SwipeDirection::Down;
    } else if (delta.y() < 0) {
        direction = SwipeDirection::Up;
    }

    bool _ = false;
    const auto ret = updateGesture(GestureType::Wheel, {
        .delta = std::abs(delta.x()) > 0 ? delta.x() : delta.y(),
        .direction = static_cast<uint32_t>(direction),
        .ended = &_
    });

    auto end = std::chrono::steady_clock::now();
    qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "EventProcessed<Wheel>(hasGestures: " << ret << ", time: " << since(start, end) << ")";
    return ret;
}

void GestureHandler::pointerMotion(const QPointF &delta)
{
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Event<PointerMotion>(delta: " << delta << ")";
    auto start = std::chrono::steady_clock::now();

    if (Input::implementation()->isSendingInput()) {
        auto end = std::chrono::steady_clock::now();
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "EventProcessed<PointerMotion>(status: Ignored, time: " << since(start, end) << ")";
        return;
    }

    if (m_mouseButtonTimer.isActive() || m_mouseTimeoutTimer.isActive()) {
        gestureCancel(GestureType::All);

        m_mouseButtonTimer.stop();
        m_mouseTimeoutTimer.stop();

        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "ActivatingMotionGestures";
        if (!gestureBegin(GestureType::Stroke | GestureType::Swipe)) {
            qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "NoMotionGestures";
            pressBlockedMouseButtons();
        }
    } else if (!hasActiveGestures(GestureType::Stroke | GestureType::Swipe) && Input::implementation()->keyboardModifiers()) {
        gestureBegin(GestureType::Stroke | GestureType::Swipe);
    }

    const auto hadActiveGestures = hasActiveGestures(GestureType::Stroke | GestureType::Swipe);
    bool _ = false;
    touchMotion(delta, _);
    if (hadActiveGestures && !hasActiveGestures(GestureType::Stroke | GestureType::Swipe)) {
        // Swipe gesture cancelled due to wrong speed or direction
        pressBlockedMouseButtons();
    }
}

bool GestureHandler::pointerButton(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state)
{
    qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "Event<PointerButton>(button: " << button << ", state: " << state << ")";
    auto start = std::chrono::steady_clock::now();

    if (Input::implementation()->isSendingInput()) {
        auto end = std::chrono::steady_clock::now();
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "EventProcessed<PointerButton>(status: Ignored, time: " << since(start, end) << ")";
        return false;
    }

    if (state) {
        gestureCancel(GestureType::All);

        m_data = {};

        // This should be per-gesture instead of global, but it's good enough
        m_instantPress = false;
        for (const auto &gesture : gestures(GestureType::Press, m_data)) {
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
    if (m_deviceType == DeviceType::Mouse) {
        gestureBegin(GestureType::Wheel);
        wheelGestureUpdate(delta);
        if (gestureEnd(GestureType::Wheel)) {
            m_mouseLongPointerAxisTimeoutTimer.start(s_longPointerAxisTimeout);
            return true;
        }
        return false;
    }
    const auto mouse = m_deviceType == DeviceType::Mouse;

    if (!m_pointerAxisTimeoutTimer.isActive()) {
        gestureBegin(GestureType::Stroke | GestureType::Swipe, 2);
    }
    m_pointerAxisTimeoutTimer.stop();
    m_pointerAxisTimeoutTimer.start(s_touchpadPointerAxisTimeout);

    bool _ = false;
    if (touchMotion(delta, _)) {
        return true;
    }
    m_pointerAxisTimeoutTimer.stop();
    return false;
}

void GestureHandler::keyboardKey(const Qt::Key &key, const bool &state)
{
    // Lazy way to check for modifier release to end mouse motion gestures
    if (Input::implementation()->isSendingInput() || m_deviceType != DeviceType::Mouse || state) {
        return;
    }

    gestureEnd(GestureType::All);
}

bool GestureHandler::shouldBlockMouseButton(const Qt::MouseButton &button, const GestureBeginEvent &event)
{
    for (const auto &gesture : m_gestures) {
        const auto gestureModifiers = gesture->keyboardModifiers();
        const auto gestureButtons = gesture->mouseButtons();
        const auto gestureEdges = gesture->edges();
        if ((gestureModifiers && *gestureModifiers != event.keyboardModifiers)
            || (gestureEdges && !gestureEdges->contains(event.edges))) {
            continue;
        }
        if (gestureButtons && (*gestureButtons & button)) {
            qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "MouseButtonBlocked(button: " << button << ", gesture: " << gesture->name() << ")";
            return true;
        }
    }
    return false;
}

void GestureHandler::pressBlockedMouseButtons()
{
    for (const auto &button : m_blockedMouseButtons) {
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "MouseButtonUnblocking(button: " << button << ")";
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
    qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "GesturesActivating(types: " << types << ", fingers: " << data.fingers << ", mouseButtons: " << data.mouseButtons << ", keyboardModifiers: " << data.keyboardModifiers << ", edges: " << data.edges << ")";
    gestureCancel(GestureType::All);
    resetMembers();

    auto hasKeyboardModifiers = false;
    for (auto &gesture : gestures(types, data)) {
        if (gesture->type() == GestureType::Press && !m_pressTimer.isActive()) {
            qCDebug(LIBGESTURES_GESTURE_HANDLER) << "PressTimerStarted";
            m_pressTimer.start(s_holdDelta);
        }

        m_isDeterminingSpeed = m_isDeterminingSpeed || gesture->speed() != GestureSpeed::Any;
        hasKeyboardModifiers = hasKeyboardModifiers || (gesture->keyboardModifiers() && *gesture->keyboardModifiers() != Qt::KeyboardModifier::NoModifier);

        m_activeGestures.push_back(gesture);
        qCDebug(LIBGESTURES_GESTURE_HANDLER) << qPrintable(QString("GestureActivated(name: \"%1\"").arg(gesture->name()));
    }
    if (hasKeyboardModifiers) {
        Input::implementation()->keyboardClearModifiers();
    }

    const auto gestures = m_activeGestures.size();
    qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "GesturesActivated(count: " << gestures << ", hasSpeed: " << m_isDeterminingSpeed << ", hasModifiers: " << hasKeyboardModifiers << ")";
    return gestures != 0;
}

bool GestureHandler::gestureEnd(const GestureTypes &types)
{
    return gestureEndOrCancel(types);
}

bool GestureHandler::gestureCancel(const GestureTypes &types)
{
    return gestureEndOrCancel(types, false);
}

void GestureHandler::gestureCancel(Gesture *except)
{
    qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote().nospace() << "GesturesCancelling(except: " << except->name() << ")";
    for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();) {
        auto gesture = *it;
        if (gesture != except) {
            gesture->cancel();
            it = m_activeGestures.erase(it);
            continue;
        }
        it++;
    }
}

bool GestureHandler::gestureEndOrCancel(const GestureTypes &types, const bool &end)
{
    if (end) {
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "GesturesEnding(types: " << types << ")";
    } else {
        qCDebug(LIBGESTURES_GESTURE_HANDLER).nospace() << "GesturesCancelling(types: " << types << ")";
    }

    m_mouseLongPointerAxisTimeoutTimer.stop();

    auto active = activeGestures(types);
    auto hadActiveGestures = !active.empty();

    if (types & GestureType::Press && m_pressTimer.isActive()) {
        qCDebug(LIBGESTURES_GESTURE_HANDLER) << "HoldTimerStopped";
        m_pressTimer.stop();
    }
    if (end && types & GestureType::Stroke) {
        auto start = std::chrono::steady_clock::now();
        const Stroke stroke(m_stroke);
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote()
            << QString("StrokeConstructed(points: %1, deltas: %2, time: %3)").arg(QString::number(stroke.points().size()), QString::number(m_stroke.size()), since(start));

        Gesture *bestGesture = nullptr;
        double bestScore = 0;
        const auto &gestures = activeGestures(GestureType::Stroke);

        start = std::chrono::steady_clock::now();
        for (const auto &gesture : activeGestures(GestureType::Stroke)) {
            const auto score = stroke.compare(static_cast<StrokeGesture *>(gesture)->stroke());
            if (score > bestScore) {
                bestGesture = gesture;
                bestScore = score;
            }
        }
        qCDebug(LIBGESTURES_GESTURE_HANDLER).noquote()
            << QString("StrokeCompared(strokes: %1, bestScore: %2, time: %3)").arg(QString::number(gestures.size()), QString::number(bestScore), since(start));

        gestureCancel(bestGesture);
        if (bestGesture != nullptr && bestScore >= Stroke::min_matching_score()) {
            bestGesture->end();
        }
    }

    for (auto it = m_activeGestures.begin(); it != m_activeGestures.end();) {
        auto gesture = *it;
        if (!(types & gesture->type())) {
            it++;
            continue;
        }

        it = m_activeGestures.erase(it);
        if (end) {
            if (gesture->shouldCancelOtherGestures(true)) {
                gestureCancel(gesture);
                gesture->end();
                break;
            }

            continue;
        }

        gesture->cancel();
    }

    return hadActiveGestures;
}

void GestureHandler::resetMembers()
{
    m_stroke.clear();
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