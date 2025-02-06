#pragma once

#include "holdgesture.h"
#include "pinchgesture.h"
#include "swipegesture.h"

#include <QObject>
#include <QPointF>

namespace libgestures
{

enum Axis {
    Horizontal,
    Vertical,
    None
};

/**
 * Recognizes and handles gestures based on received input events.
 */
class GestureRecognizer : public QObject
{
    Q_OBJECT
public:
    GestureRecognizer() = default;

    /**
     * Adds a gesture to the end of the gesture list.
     * @remark This method doesn't prevent duplicate gestures from being added.
     */
    void registerGesture(std::shared_ptr<Gesture> gesture);

    /**
     * Removes all registered gestures.
     */
    void unregisterGestures();

    GestureRecognizer &setInputEventsToSample(const uint8_t &events);
    GestureRecognizer &setSwipeFastThreshold(const qreal &threshold);
    GestureRecognizer &setPinchInFastThreshold(const qreal &threshold);
    GestureRecognizer &setPinchOutFastThreshold(const qreal &threshold);

    GestureRecognizer &setDeltaMultiplier(const qreal &multiplier);

    /**
     * @param fingerCount Amount of fingers currently on the input device.
     */
    void holdGestureBegin(const uint8_t &fingerCount);
    // TODO move this back to GestureRecognizer
    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @remark This method should be called every 1 millisecond.
     */
    void holdGestureUpdate(const qreal &delta, bool &endedPrematurely);
    /**
     * @return Whether there were any active hold gestures before the end.
     */
    bool holdGestureEnd();
    void holdGestureCancel();

    /**
     * @param fingerCount Amount of fingers currently on the input device.
     */
    void swipeGestureBegin(const uint8_t &fingerCount);
    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @return Whether there are currently any active swipe gestures.
     */
    bool swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely);
    /**
     * @remark This method may be called before the fingers have been lifted if a gesture with a threshold has been
     * triggered.
     * @return Whether there were any active swipe gestures before the end.
     */
    bool swipeGestureEnd();
    void swipeGestureCancel();

    /**
     * @param fingerCount Amount of fingers currently on the input device.
     */
    void pinchGestureBegin(const uint8_t &fingerCount);
    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted. This
     * parameter is only handled in the KWin effect to continue blocking built-in gestures.
     * @return Whether there are currently any active pinch gestures.
     */
    bool pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely);
    /**
     * @param resetHasTriggeredGesture Whether built-in gestures should be allowed to trigger. Should be @c false if
     * this method is called due to a gesture with a threshold triggering.
     * @remarks This method may be called before the fingers have been lifted if a gesture with a threshold has been
     * triggered.
     * @return Whether there were any active pinch gestures before the end.
     */
    bool pinchGestureEnd();
    void pinchGestureCancel();

private:
    template<class TGesture>
    void gestureBegin(const uint8_t &fingerCount, std::vector<std::shared_ptr<TGesture>> &activeGestures);
    template<class TGesture>
    bool gestureEnd(std::vector<std::shared_ptr<TGesture>> &activeGestures);
    template<class TGesture>
    void gestureCancel(std::vector<std::shared_ptr<TGesture>> &activeGestures);

    void resetMembers();

    std::vector<std::shared_ptr<Gesture>> m_gestures;

    std::vector<std::shared_ptr<SwipeGesture>> m_activeSwipeGestures;
    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;

    std::vector<std::shared_ptr<PinchGesture>> m_activePinchGestures;
    qreal m_previousPinchScale = 1;

    std::vector<std::shared_ptr<HoldGesture>> m_activeHoldGestures;

    uint8_t m_inputEventsToSample = 3;
    qreal m_swipeGestureFastThreshold = 20;
    qreal m_pinchInFastThreshold = 0.04;
    qreal m_pinchOutFastThreshold = 0.08;

    GestureSpeed m_speed = GestureSpeed::Any;
    bool m_isDeterminingSpeed = false;
    uint8_t m_sampledInputEvents = 0;
    qreal m_accumulatedAbsoluteSampledDelta = 0;

    qreal m_deltaMultiplier = 1.0;

    friend struct YAML::convert<std::shared_ptr<GestureRecognizer>>;
    friend class TestGestureRecognizer;
};

}