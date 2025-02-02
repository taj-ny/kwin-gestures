#pragma once

#include "libgestures/condition.h"

#include <QPointF>

namespace libgestures
{

/**
 * At which point during the gesture the action should be executed.
 */
enum On {
    /**
     * The gesture has ended.
     */
    End,

    /**
     * The gesture has started. If a threshold has been set on the gesture, the action will be triggered when it's
     * reached. Begin actions can't have a minimum threshold.
     */
    Begin,

    /**
     * An input event has been sent by the device.
     */
    Update,

    /**
     * The gesture has been cancelled, due to the direction changing or the finger count changing but not to 0.
     */
    Cancel,

    /**
     * The gesture has ended or has been cancelled.
     */
    EndOrCancel
};

/**
 * Executed at a particular point during the gesture if the specified conditions are met.
 */
class GestureAction : public QObject
{
    Q_OBJECT

public:
    /**
     * Executes the action if conditions are satisfied and the threshold reached. Does nothing otherwise.
     * @return Whether the action has been executed.
     */
    virtual bool tryExecute();

    /**
     * @return Whether the action satisfies at least one condition, or no conditions have been added.
     */
    [[nodiscard]] bool satisfiesConditions() const;

    /**
     * Whether this action can be executed during a gesture, not necessarily right now.
     * @remark @c execute() calls this method.
     * @remark This method doesn't check whether the threshold has been reached.
     * @return @c true if repeat interval is set or the action hasn't been triggered during a gesture, @c false
     * otherwise or when none of the specified conditions have been satisfied.
     */
    [[nodiscard]] bool canExecute() const;

    /**
     * @return Whether any other actions belonging to a gesture should not be executed. @c true if the action has been
     * executed and blockOtherActions has been set to @c true, @c false otherwise.
     */
    [[nodiscard]] bool blocksOtherActions() const;

    /**
     * At least one condition (or zero if none added) has to be satisfied in order for this action to be executed.
     */
    void addCondition(const std::shared_ptr<const Condition> &condition);

    /**
     * @param blockOtherActions Whether this action should block all other actions, including actions belonging to
     * other custom and built-in gestures, from being executed during the gesture, if the action is executed.
     */
    void setBlockOtherActions(const bool &blockOtherActions);

    /**
     * @param interval How often an update action should repeat.
     * If 0, the action will be executed exactly once per input event.
     * If not 0, the action will only be executed when the accumulated delta reaches this interval, and will keep
     * being executed until the delta is smaller than the interval.
     */
    void setRepeatInterval(const qreal &interval);

    /**
     * Sets how far the gesture needs to progress in order for the action to be executed. Thresholds are always
     * positive no matter the direction. 0 is no threshold.
     * @remark Begin actions can't have thresholds. Set the threshold on the gesture instead.
     */
    void setThresholds(const qreal &minimum, const qreal &maximum);

    /**
     * @param on At which point during the gesture the action should be executed.
     */
    void setOn(const On &on);
signals:
    /**
     * Emitted when the action has been executed.
     */
    void executed();

    /**
     * Emitted when the gesture this action belongs to has been cancelled.
     */
    void gestureCancelled();

    /*
     * Emitted when the gesture this action belongs to has ended.
     */
    void gestureEnded();

    /**
     * Emitted when the gesture this action belongs to has ended has been updated, its threshold reached and
     * the action's conditions satisfied.
     */
    void gestureStarted();

    /**
     * Emitted when the gesture this action belongs to has been updated.
     */
    void gestureUpdated(const qreal &delta, const QPointF &deltaPointMultiplied);

protected:
    GestureAction();

    // This is just a quick way to get directionless swipe gestures working
    QPointF m_currentDeltaPointMultiplied;
private slots:
    void onGestureCancelled();
    void onGestureEnded();
    void onGestureStarted();
    void onGestureUpdated(const qreal &delta, const QPointF &deltaPointMultiplied);

private:
    /**
     * @return Whether the accumulated delta fits within the specified range.
     */
    [[nodiscard]] bool thresholdReached() const;

    qreal m_repeatInterval = 0;

    bool m_blockOtherActions = false;
    qreal m_minimumThreshold = 0;
    qreal m_maximumThreshold = 0;

    std::vector<std::shared_ptr<const Condition>> m_conditions;

    /**
     * The sum of deltas from each update event.
     *
     * Reset when the direction changes, the gesture begins, ends or is cancelled.
     */
    qreal m_accumulatedDelta = 0;
    /**
     * The sum of absolute deltas from each update event. Used for thresholds.
     *
     * Reset when the gesture begins, ends or is cancelled.
     */
    qreal m_absoluteAccumulatedDelta = 0;

    /**
     * Whether the action has been executed during the gesture.
     *
     * Reset when the gesture begins, ends or is cancelled.
     */
    bool m_executed = false;

    On m_on = On::Update;

    friend class TestGestureRecognizer;
};

}