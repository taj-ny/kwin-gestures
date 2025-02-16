#pragma once

#include "libgestures/actions/action.h"

#include <QTimer>

namespace libgestures
{

enum GestureSpeed {
    Any,
    Slow,
    Fast
};

class Gesture : public QObject
{
    Q_OBJECT
public:
    Gesture();

    /**
     * @returns Whether the amount of fingers fits within the specified range, all (if any) conditions are
     * satisfied, and there is at least one action (if any) that satisfies conditions.
     */
    bool satisfiesConditions(const uint8_t &fingerCount) const;

    void setBlocksOthers(const bool &blocksOthers);
    const bool &blocksOthers() const;

    GestureSpeed speed() const;

    void addAction(std::unique_ptr<GestureAction> action);

    void addCondition(const std::shared_ptr<const Condition> &condition);

    void setSpeed(const GestureSpeed &speed);
    void setThresholds(const qreal &minimum, const qreal &maximum);
    void setFingers(const uint8_t &minimum, const uint8_t &maximum);

    void setInertiaFriction(const qreal &friction);

    /**
     * @param minimumDelta The point at which a 1:1 gesture should be ended, instead of cancelled, when fingers are
     * lifted.
     */
    void setMinimumDelta(const qreal &delta);

    /**
     * @param maximumDelta The point at which a 1:1 gesture should be ended automatically. Ignored if 0
     */
    void setMaximumDelta(const qreal &delta);
signals:
    /**
     * Emitted when the gesture has been cancelled.
     */
    void cancelled();

    /**
     * Emitted when the gesture has ended.
     */
    void ended();

    /**
     * Emitted when the gesture has started and reached the specified threshold. If no threshold has been specified, the
     * signal will be emitted immediately.
     */
    void started();

    /**
     * Emitted when the gesture has been updated.
     *
     * @param endedPrematurely Whether the gesture recognizer should end the gesture. Used when only one action can be
     * triggered.
     */
    void updated(const QPointF &delta, const QPointF &deltaPointMultiplied, bool &endedPrematurely);

protected:
    /**
     * @return The delta for the specified input event's delta.
     * For single-directional gestures, the delta should be positive if the event direction matches with the gesture's
     * direction, otherwise it should be negative.
     * For bi-directional gestures, the delta should be positive if the event direction matches the last direction in
     * the enum (e.g for SwipeDirection::LeftRight, right will be positive), otherwise it should be negative.
     * For SwipeDirection::Any, it doesn't matter what is returned for now.
     */
    virtual qreal getRealDelta(const QPointF &delta) const;
private slots:
    void onCancelled();
    void onEnded();
    void onStarted();
    void onUpdated(const QPointF &delta, const QPointF &deltaPointMultiplied, bool &endedPrematurely);
    void onUpdatedInertia();

private:
    /**
     * @return Whether the accumulated delta fits within the specified range.
     */
    bool thresholdReached() const;

    uint8_t m_minimumFingers = 0;
    uint8_t m_maximumFingers = 0;
    qreal m_minimumThreshold = 0;
    qreal m_maximumThreshold = 0;
    qreal m_minimumDelta = 0;
    qreal m_maximumDelta = 0;
    GestureSpeed m_speed = GestureSpeed::Any;

    bool m_blocksOthers = false;

    std::vector<std::shared_ptr<const Condition>> m_conditions;

    std::vector<std::unique_ptr<GestureAction>> m_actions;

    qreal m_absoluteAccumulatedDelta = 0;
    qreal m_accumulatedDelta = 0;
    bool m_hasStarted = false;

    bool m_inertial = true;
    QPointF m_velocity;
    QTimer m_inertiaTimer;
    uint32_t m_inertiaEvents = 0;
    qreal m_inertiaFriction = 0.9;

    friend class BuiltinGesture;
};

}