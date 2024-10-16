#pragma once

#include "condition.h"

enum When
{
    Cancelled,
    Ended,
    Started,
    Updated
};

class GestureAction
{
public:
    GestureAction(qreal repeatInterval);
    virtual ~GestureAction() = default;

    virtual void execute();
    bool canExecute() const;

    bool repeat() const { return m_repeatInterval != 0; };

    /**
     * Called when the gesture has been cancelled.
     */
    void cancelled();

    /**
     * Called when the gesture has ended.
     */
    void ended();

    /**
     * Called when the gesture has started.
     */
    void started();

    /**
     * Called when the gesture has been updated.
     */
    void update(const qreal &delta);

    void addCondition(const Condition &condition);

    /**
     * @returns Whether the action satisfies at least one condition specified by the user, @c true when no conditions
     * were specified.
     */
    bool satisfiesConditions() const;
private:
    qreal m_repeatInterval;
    std::vector<Condition> m_conditions;

    qreal m_accumulatedDelta = 0;
    bool m_triggered = false;
    When m_when = When::Updated;

    friend class TestGestureRecognizerHold;
};