#pragma once

#include "press.h"
#include "pinch.h"
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

enum class GestureType : uint32_t {
    Press = 1u << 0,
    Pinch = 1u << 1,
    Swipe = 1u << 2,
    Wheel = 1u << 3,
    Rotate = 1u << 4,
    All = UINT32_MAX
};
Q_DECLARE_FLAGS(GestureTypes, GestureType)
Q_DECLARE_OPERATORS_FOR_FLAGS(GestureTypes)

enum class DeviceType {
    Mouse,
    Touchpad
};

struct GestureHandlerUpdateEvent
{
    bool endedPrematurely = false;

};



/**
 * Recognizes and handles gestures based on received input events. Can only handle one type of gesture at a time. Each
 * type of input device (keyboard, mouse, touchscreen) must have its own instance.
 *
 * Supported gestures:
 *   - mouse - hold, swipe, wheel
 *   - touchpad - hold, pinch, swipe, rotate
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
    std::vector<Gesture *> activatableGestures(const GestureType &type, const GestureBeginEvent &data = {});
    bool hasActiveGestures(const GestureType &type);

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
     * @return Whether there are currently any active swipe gestures.
     */
    bool swipeGestureUpdate(const QPointF &delta, bool &ended);

    /**
     * @param ended Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @return Whether there are currently any active pinch gestures.
     */
    bool pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &ended);

    void pointerMotion(const QPointF &delta);
    bool pointerButton(const Qt::MouseButton &button, const quint32 &nativeButton, const bool &state);
    bool pointerAxis(const QPointF &delta);

    void keyboardModifier(const Qt::KeyboardModifier &modifier, const bool &state);

private:

    bool shouldBlockMouseButton(const Qt::MouseButton &button, const GestureBeginEvent &event);
    void pressBlockedMouseButtons();

    void pressGestureUpdate(const qreal &delta);
    bool wheelGestureUpdate(const QPointF &delta);

    bool gestureEndOrCancel(const GestureTypes &types, const bool &end = true);

    void resetMembers();

    DeviceType m_deviceType = DeviceType::Touchpad;

    std::map<GestureType, std::vector<std::shared_ptr<Gesture>>> m_gestures;
    std::map<GestureType, std::vector<Gesture*>> m_activeGestures;

    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;
    QTimer m_pointerAxisTimeoutTimer;
    QTimer m_mouseLongPointerAxisTimeoutTimer; // Used to block mouse buttons

    qreal m_previousPinchScale = 1;

    PinchType m_pinchType = PinchType::Unknown;
    qreal m_accumulatedRotateDelta = 0;

    QTimer m_pressTimer;

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

    qreal m_deltaMultiplier = 1.0;

    friend struct YAML::convert<std::shared_ptr<GestureHandler>>;
    friend class TestGestureRecognizer;
};

}