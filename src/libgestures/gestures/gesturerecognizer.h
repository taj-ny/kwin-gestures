#pragma once

#include "gestures/holdgesture.h"
#include "gestures/pinchgesture.h"
#include "gestures/swipegesture.h"
#include <QObject>
#include <QPointF>

enum Axis
{
    Horizontal,
    Vertical,
    None
};

/**
 * Recognizes and triggers gestures based on events received from GestureInputEventFilter. Gestures are triggered in
 * order as they were registered.
 */
class GestureRecognizer : public QObject
{
    Q_OBJECT
public:
    /**
     * Adds a gesture to the ended of the gesture list.
     * @remark This method doesn't ensure that duplicate
     */
    void registerGesture(std::shared_ptr<Gesture> gesture);

    /**
     * Removes all registered gestures.
     */
    void unregisterGestures();

    void holdGestureBegin(const uint8_t &fingerCount);
    /**
     * @param endedPrematurely Whether the gesture should end immediately before the fingers have been lifted.
     * If true, GestureInputEventFilter will send the swipeGestureCancelled event to all filters.
     * Passed by reference.
     */
    void holdGestureUpdate(const qreal &delta, bool &endedPrematurely);
    /**
     * @return @c true if GlobalShortcutFilter shouldn't process this gesture event, @c false otherwise.
     */
    bool holdGestureEnd();
    void holdGestureCancelled();

    void swipeGestureBegin(const uint8_t &fingerCount);
    /**
     * @return @c true if GlobalShortcutFilter shouldn't process this gesture event, @c false otherwise.
     */
    bool swipeGestureUpdate(const QPointF &delta, bool &endedPrematurely);
    /**
     * @param resetHasTriggeredGesture Whether built-in gestures should be allowed to trigger. Should be @c false if
     * this method is called due to a gesture with a threshold triggering.
     * @remarks This method may be called before the fingers have been lifted if a gesture with a threshold has been
     * triggered.
     * @return @c true if GlobalShortcutFilter shouldn't process this gesture event, @c false otherwise.
     */
    bool swipeGestureEnd();
    void swipeGestureCancelled();

    void pinchGestureBegin(const uint8_t &fingerCount);
    /**
     * @return @c true if GlobalShortcutFilter shouldn't process this gesture event, @c false otherwise.
     */
    bool pinchGestureUpdate(const qreal &scale, const qreal &angleDelta, const QPointF &delta, bool &endedPrematurely);
    /**
     * @param resetHasTriggeredGesture Whether built-in gestures should be allowed to trigger. Should be @c false if
     * this method is called due to a gesture with a threshold triggering.
     * @remarks This method may be called before the fingers have been lifted if a gesture with a threshold has been
     * triggered.
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