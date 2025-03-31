#pragma once

#include "press.h"
#include "pinch.h"
#include "stroke.h"
#include "rotate.h"
#include "wheel.h"

#include <deque>

#include <QObject>
#include <QPointF>
#include <QTimer>

namespace libgestures
{

enum class Axis {
    Horizontal,
    Vertical,
    None
};

enum class PinchType {
    Unknown,
    Pinch,
    Rotate
};

enum class DeviceType {
    Mouse,
    Touchpad
};

struct GestureUpdateEvent
{
    qreal delta = 0;
    uint32_t direction = 0;
    bool *ended;
    QPointF deltaPointMultiplied{};
};

/**
 * Recognizes and handles gestures based on received input events. Can only handle one type of gesture at a time. Each
 * type of input device (keyboard, mouse, touchscreen) must have its own instance.
 *
 * Supported gestures:
 *   - mouse - hold, swipe, wheel
 *   - touchpad - hold, pinch, swipe, rotate
 *
 * TODO Requires a major refactor
 */
class GestureHandler : public QObject
{
    Q_OBJECT
public:
    GestureHandler();

    /**
     * Default device type is Touchpad.
     */
    void setDeviceType(const DeviceType &type);

    /**
     * Adds a gesture to the end of the gesture list.
     * @remark This method doesn't prevent duplicate gestures from being added.
     */
    void registerGesture(std::shared_ptr<Gesture> gesture);

    /**
     * @return Gestures of the specified type that satisfy begin conditions and are eligible for activation.
     */
    std::vector<Gesture *> gestures(const GestureTypes &types, const std::optional<GestureBeginEvent> &data = std::nullopt);
    std::vector<Gesture *> activeGestures(const GestureTypes &types);
    bool hasActiveGestures(const GestureTypes &types);

    void setInputEventsToSample(const uint8_t &events);
    void setSwipeFastThreshold(const qreal &threshold);
    void setPinchInFastThreshold(const qreal &threshold);
    void setPinchOutFastThreshold(const qreal &threshold);
    void setRotateFastThreshold(const qreal &threshold);

    void setDeltaMultiplier(const qreal &multiplier);

    /**
     * Cancels all gestures and activates the specified ones.
     * @return Whether any gestures of the specified types have been activated.
     */
    bool gestureBegin(const GestureTypes &types, const GestureBeginEvent &data = {});
    /**
     * Cancels all gestures and activates the specified ones.
     * @return Whether any gestures of the specified types have been activated.
     */
    bool gestureBegin(const GestureTypes &types, const uint8_t &fingers);
    /**
     * Ends the specified gestures.
     * @return Whether there were any active gestures of the specified types.
     */
    bool gestureEnd(const GestureTypes &types);
    /**
     * Cancels the specified gestures.
     * @return Whether there were any active gestures of the specified types.
     */
    bool gestureCancel(const GestureTypes &types);

    /**
     * @param ended Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @return Whether there are currently any active pinch gestures.
     */
    bool pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &ended);

    /**
     * Handles stroke and swipe gestures.
     * @param delta
     * @param ended
     * @return
     */
    bool touchMotion(const QPointF &delta, bool &ended);

    void pointerMotion(const QPointF &delta);
    bool pointerButton(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state);
    /**
     * Touchpad or mouse scroll. Handles mouse wheel and 2-finger touchpad motion gestures.
     * @return Whether any gestures have been activated or are active.
     */
    bool pointerAxis(const QPointF &delta);

    void keyboardKey(const Qt::Key &key, const bool &state);

    bool shouldBlockMouseButton(const Qt::MouseButton &button);
    void pressBlockedMouseButtons();

    void pressGestureUpdate(const qreal &delta);
    bool wheelGestureUpdate(const QPointF &delta);

    /**
     * Cancels all gestures except one.
     * @param except The gesture to not cancel.
     */
    void gestureCancel(Gesture *except);
    bool gestureEndOrCancel(const GestureTypes &types, const bool &end = true);

    /**
     * Updates gestures of multiple types in order as specified in the configuration file.
     * @return Whether there are any active gestures.
     */
    bool updateGesture(const std::map<GestureType, GestureUpdateEvent> &events);
    /**
     * Updates gestures of the specified type in order as specified in the configuration file.
     * @return Whether there are any active gestures.
     */
    bool updateGesture(const GestureType &type, GestureUpdateEvent event);

    bool determineSpeed(const qreal &delta, const qreal &fastThreshold);
    void resetMembers();

    DeviceType m_deviceType = DeviceType::Touchpad;

    std::vector<std::shared_ptr<Gesture>> m_gestures;
    std::vector<Gesture *> m_activeGestures;

    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;
    QTimer m_pointerAxisTimeoutTimer;
    QTimer m_mouseLongPointerAxisTimeoutTimer; // Used to block mouse buttons

    qreal m_previousPinchScale = 1;

    PinchType m_pinchType = PinchType::Unknown;
    qreal m_accumulatedRotateDelta = 0;

    QTimer m_pressTimer;

    std::vector<QPointF> m_stroke;

    /**
     * Used to wait until all mouse buttons have been pressed to avoid conflicts with gestures that require more than
     * one button.
     */
    QTimer m_mouseButtonTimer;
    QTimer m_mouseTimeoutTimer;
    GestureBeginEvent m_data;
    bool m_instantPress = false;

    QList<quint32> m_blockedMouseButtons;

    uint8_t m_inputEventsToSample = 3;
    qreal m_swipeGestureFastThreshold = 20;
    qreal m_pinchInFastThreshold = 0.04;
    qreal m_pinchOutFastThreshold = 0.08;
    qreal m_rotateFastThreshold = 5;

    GestureSpeed m_speed = GestureSpeed::Any;
    bool m_isDeterminingSpeed = false;
    uint8_t m_sampledInputEvents = 0;
    qreal m_accumulatedAbsoluteSampledDelta = 0;

    qreal m_mouseMotionSinceButtonPress = 0;
    /**
     * Set to true when any gesture is updated, prevents mouse buttons from being unblocked. Reset when all buttons
     * are released.
     */
    bool m_dontUnblockMouseButtons = false;

    qreal m_deltaMultiplier = 1.0;

    bool m_conflictsResolved = false;

    friend struct YAML::convert<std::shared_ptr<GestureHandler>>;
    friend class TestGestureRecognizer;
};

}