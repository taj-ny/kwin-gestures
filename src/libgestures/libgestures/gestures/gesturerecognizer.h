#pragma once

#include "holdgesture.h"
#include "pinchgesture.h"
#include "swipegesture.h"
#include <QObject>
#include <QPointF>

namespace libgestures
{

enum Axis
{
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
    /**
     * Adds a gesture to the end of the gesture list.
     * @remark This method doesn't prevent duplicate gestures from being added.
     */
    void registerGesture(std::shared_ptr<Gesture> gesture);

    /**
     * Removes all registered gestures.
     */
    void unregisterGestures();

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
    void holdGestureCancelled();

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
    void swipeGestureCancelled();

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
    void pinchGestureCancelled();
private:
    std::vector<std::shared_ptr<Gesture>> m_gestures;

    QList<std::shared_ptr<SwipeGesture>> m_activeSwipeGestures;
    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;

    QList<std::shared_ptr<PinchGesture>> m_activePinchGestures;
    qreal m_previousPinchScale = 1;

    QList<std::shared_ptr<Gesture>> m_activeHoldGestures;
    uint m_currentFingerCount = 0;

    friend class TestGestureRecognizerHold;
};

} // namespace libgestures