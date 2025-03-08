#pragma once

#include "hold.h"
#include "pinch.h"
#include "rotate.h"
#include "wheel.h"

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

enum class GestureType {
    Hold,
    Pinch,
    Rotate,
    Swipe,
    Wheel
};

enum class DeviceType {
    Mouse,
    Touchpad
};

/**
 * Recognizes and handles gestures based on received input events. Can only handle one gesture at a time. Each type of
 * input device (keyboard, mouse, touchscreen) must have its own instance.
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

    void setDeviceType(const DeviceType &type);

    /**
     * Adds a gesture to the end of the gesture list.
     * @remark This method doesn't prevent duplicate gestures from being added.
     */
    void registerGesture(std::shared_ptr<Gesture> gesture);

    /**
     * Removes all registered gestures.
     */
    void unregisterGestures();

    bool hasActiveGestures();

    void setInputEventsToSample(const uint8_t &events);
    void setSwipeFastThreshold(const qreal &threshold);
    void setPinchInFastThreshold(const qreal &threshold);
    void setPinchOutFastThreshold(const qreal &threshold);
    void setRotateFastThreshold(const qreal &threshold);

    void setDeltaMultiplier(const qreal &multiplier);

    bool hasActivatableGestures(const GestureType &type, const uint8_t &fingerCount = 0);
    /**
     * @param fingerCount Amount of fingers currently on the input device.
     * @return Whether any gestures have been activated.
     */
    bool gestureBegin(const GestureType &type, const uint8_t &fingerCount = 0);
    /**
     * @return Whether there were any active gestures before the end.
     */
    bool gestureEnd(const GestureType &type);
    void gestureCancel(const GestureType &type);

    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @return Whether there are currently any active swipe gestures.
     */
    bool swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely);

    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @return Whether there are currently any active pinch gestures.
     */
    bool pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely);

    /**
     * @return Whether any gestures have been activated.
     */
    bool wheelGestureUpdate(const QPointF &delta);

    void pointerMotion(const QPointF &delta);
    bool pointerButton(const bool &state);
    bool pointerAxis(const QPointF &delta);

private:
    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     */
    void holdGestureUpdate(const qreal &delta, bool &endedPrematurely);

    /**
     * @tparam TGesture
     * @param gestures Type of gestures eligible for activation.
     * @param activeGestures Vector where activated gestures will be inserted.
     * @return Whether any gestures have been activated.
     */
    template<class TGesture>
    bool gestureBegin(const uint8_t &fingerCount, const GestureType &type, std::vector<std::shared_ptr<TGesture>> &activeGestures);
    template<class TGesture>
    bool gestureEnd(std::vector<std::shared_ptr<TGesture>> &activeGestures);
    template<class TGesture>
    void gestureCancel(std::vector<std::shared_ptr<TGesture>> &activeGestures);

    void resetMembers();

    DeviceType m_deviceType = DeviceType::Touchpad;

    std::map<GestureType, std::vector<std::shared_ptr<Gesture>>> m_gestures;

    std::vector<std::shared_ptr<SwipeGesture>> m_activeSwipeGestures;
    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;
    QTimer m_pointerAxisTimer;

    std::vector<std::shared_ptr<PinchGesture>> m_activePinchGestures;
    qreal m_previousPinchScale = 1;

    std::vector<std::shared_ptr<RotateGesture>> m_activeRotateGestures;
    PinchType m_pinchType = PinchType::Unknown;
    qreal m_accumulatedRotateDelta = 0;

    std::vector<std::shared_ptr<HoldGesture>> m_activeHoldGestures;
    QTimer m_holdTimer;

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