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

enum class IntervalDirection
{
    /**
     * The update delta can be either positive or negative.
     */
    Any,
    /**
     * The update delta must be positive.
     */
    Positive,
    /**
     * The update delta must be negative.
     */
    Negative
};

/**
 * Represents how often an action should repeat.
 */
class ActionInterval
{
public:
    const qreal &value() const;

    /**
     * @return Whether the specified delta matches the interval's direction.
     */
    bool matches(const qreal &delta) const;

    /**
     * @param value Will be converted to an absolute value. 0 means execute exactly once per input event, direction
     * still applies. Default is 0.
     */
    void setValue(const qreal &value);
    /**
     * @param direction Default is Any.
     */
    void setDirection(const IntervalDirection &direction);

private:
    qreal m_value = 0;
    IntervalDirection m_direction = IntervalDirection::Any;
};

/**
 * Executed at a specific point during the gesture if the specified conditions are met.
 */
class GestureAction
{
public:
    virtual ~GestureAction() = default;

    /**
     * Called by the gesture when it is started.
     */
    void gestureStarted();
    /**
     * Called by the gesture when it is updated.
     */
    void gestureUpdated(const qreal &delta, const QPointF &deltaPointMultiplied);
    /**
     * Called by the gesture when it has ended.
     * @param execute Whether the action should be executed.
     */
    void gestureEnded(const bool &execute);
    /**
     * Called by the gesture when it has been cancelled.
     * @param execute Whether the action should be executed.
     */
    void gestureCancelled(const bool &execute);

    /**
     * Executes the action if conditions are satisfied and the threshold reached, does nothing otherwise.
     */
    void tryExecute();
    const bool &executed() const;
    bool canExecute() const;

    /**
     * @return Whether the action satisfies at least one condition, or no conditions have been added.
     */
    bool satisfiesConditions() const;

    /**
     * @return Whether this action should block all other actions, including actions belonging to other custom and
     * built-in gestures, from being executed during the gesture, if the action is executed.
     */
    bool blocksOtherActions() const;

    /**
     * At least one condition (or zero if none added) has to be satisfied in order for this action to be executed.
     */
    void addCondition(const std::shared_ptr<const Condition> &condition);

    const QString &name() const;
    void setName(const QString &name);

    const On &on() const;
    /**
     * @param on The point during the gesture at which the action should be executed.
     */
    void setOn(const On &on);

    /**
     * @param blockOtherActions Whether this action should block all other actions, including actions belonging to
     * other custom and built-in gestures, from being executed during the gesture, if the action is executed.
     */
    void setBlockOtherActions(const bool &blockOtherActions);

    /**
     * @param interval How often an update action should repeat.
     */
    void setRepeatInterval(const ActionInterval &interval);

    /**
     * Sets how far the gesture needs to progress in order for the action to be executed. Thresholds are always
     * positive no matter the direction. 0 - no threshold.
     * @remark Begin actions can't have thresholds. Set the threshold on the gesture instead.
     */
    void setThresholds(const qreal &minimum, const qreal &maximum);

protected:
    GestureAction() = default;

    /**
     * Executes the action without checking conditions.
     */
    virtual void execute() { };

    // This is just a quick way to get directionless swipe gestures working
    QPointF m_currentDeltaPointMultiplied;

private:
    /**
     * @return Whether the accumulated delta fits within the specified range.
     */
    bool thresholdReached() const;

    void reset();

    QString m_name = "none";

    std::vector<std::shared_ptr<const Condition>> m_conditions;

    /**
     * The sum of deltas from each update event. Reset when the direction changes, the gesture begins, ends or is
     * cancelled.
     */
    qreal m_accumulatedDelta = 0;
    /**
     * The sum of absolute deltas from each update event. Used for thresholds. Reset when the gesture begins, ends or
     * is cancelled.
     */
    qreal m_absoluteAccumulatedDelta = 0;

    /**
     * Whether the action has been executed during the gesture. Reset when the gesture begins, ends or is cancelled.
     */
    bool m_executed = false;

    ActionInterval m_interval;
    bool m_blockOtherActions = false;
    qreal m_minimumThreshold = 0;
    qreal m_maximumThreshold = 0;
    On m_on = On::Update;

    friend class TestGestureRecognizer;
};

}