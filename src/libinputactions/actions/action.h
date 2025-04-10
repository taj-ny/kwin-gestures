/*
    Input Actions - Input handler that executes user-defined actions
    Copyright (C) 2024-2025 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <libinputactions/conditions/condition.h>
#include <libinputactions/range.h>

#include <memory>

#include <QLoggingCategory>
#include <QPointF>
#include <QString>

Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_ACTION)

namespace libinputactions
{

/**
 * The point of the trigger's lifecycle at which the action should be executed.
 */
enum class On {
    Begin,
    Cancel,
    End,
    EndCancel,
    Update
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
 * Defines how often and when should an action repeat.
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
 * Executed at a specific point of the trigger's lifecycle.
 */
class TriggerAction
{
public:
    virtual ~TriggerAction() = default;

    /**
     * Called by the trigger.
     */
    TEST_VIRTUAL void triggerStarted();
    /**
     * Called by the trigger.
     */
    TEST_VIRTUAL void triggerUpdated(const qreal &delta, const QPointF &deltaPointMultiplied);
    /**
     * Called by the trigger.
     */
    TEST_VIRTUAL void triggerEnded();
    /**
     * Called by the trigger.
     */
    TEST_VIRTUAL void triggerCancelled();

    /**
     * Executes the action if conditions are satisfied and the threshold reached, does nothing otherwise.
     */
    TEST_VIRTUAL void tryExecute();
    TEST_VIRTUAL const bool &executed() const;
    TEST_VIRTUAL bool canExecute() const;

    /**
     * At least one condition (or zero if none added) has to be satisfied in order for this action to be executed.
     */
    void setCondition(const std::shared_ptr<const Condition> &condition);

    const QString &name() const;
    void setName(const QString &name);

    const On &on() const;
    /**
     * @param on The point of the trigger-s lifecycle at which the action should be executed.
     */
    void setOn(const On &on);

    /**
     * @param interval How often and when an update action should repeat.
     */
    void setRepeatInterval(const ActionInterval &interval);

    /**
     * Sets how far the gesture needs to progress in order for the action to be executed. Thresholds are always
     * positive no matter the direction. 0 - no threshold.
     * @remark Begin actions can't have thresholds. Set the threshold on the gesture instead.
     */
    void setThreshold(const Range<qreal> &threshold);

protected:
    TriggerAction() = default;

    /**
     * Executes the action without checking conditions.
     */
    virtual void execute() { };

    // This is just a quick way to get directionless swipe gestures working
    QPointF m_currentDeltaPointMultiplied;

private:
    void reset();

    QString m_name = "Unnamed action";

    std::optional<std::shared_ptr<const Condition>> m_condition;

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
    std::optional<Range<qreal>> m_threshold;
    On m_on = On::Update;

    friend class TestTrigger;
};

}