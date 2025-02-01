#pragma once

#include "libgestures/condition.h"

namespace libgestures
{

enum On
{
    End,
    Begin,
    Update,
    Cancel,
    EndOrCancel
};

class GestureAction : public QObject
{
    Q_OBJECT
public:
    GestureAction();

    /**
     * Executes the action. This function calls @c canExecute() and will do nothing if it returns @false.
     * @return Whether the action has been executed.
     */
    virtual bool tryExecute();

    /**
     * Whether this action can be executed during a gesture, not necessarily right now.
     * @remark @c execute() calls this method.
     * @remark This method doesn't check whether the threshold has been reached.
     * @return @c true if repeat interval is set or the action hasn't been triggered during a gesture, @c false
     * otherwise or when none of the specified conditions have been satisfied.
     */
    [[nodiscard]] bool canExecute() const;

    [[nodiscard]] bool repeat() const { return m_repeatInterval != 0; };

    /**
     * @return Whether any other actions belonging to a gesture should not be executed. @c true if the action has been
     * executed and blockOtherActions has been set to @c true, @c false otherwise.
     */
    [[nodiscard]] bool blocksOtherActions() const;

    void addCondition(const std::shared_ptr<const Condition> &condition);

    void setBlockOtherActions(const bool &blockOtherActions);
    void setRepeatInterval(const qreal &interval);
    void setThresholds(const qreal &minimum, const qreal &maximum);
    void setOn(const On &on);
signals:
    /**
     * Emitted when the action has been executed.
     */
    void executed();

    /**
     * Emitted when the gesture this action belongs to has been cancelled.
     */
    void gestureCancelled(bool &actionExecuted);

    /*
     * Emitted when the gesture this action belongs to has ended.
     */
    void gestureEnded(bool &actionExecuted);

    /**
     * Emitted when the gesture this action belongs to has ended has been updated, its threshold reached and
     * the action's conditions satisfied.
     */
    void gestureStarted(bool &actionExecuted);

    /**
     * Emitted when the gesture this action belongs to has been updated.
     */
    void gestureUpdated(const qreal &delta, const qreal &deltaMultiplied, bool &actionExecuted);
protected:
    qreal m_currentDeltaMultiplied = 0;
private slots:
    void onGestureCancelled(bool &actionExecuted);
    void onGestureEnded(bool &actionExecuted);
    void onGestureStarted(bool &actionExecuted);
    void onGestureUpdated(const qreal &delta, const qreal &deltaMultiplied, bool &actionExecuted);
private:
    /**
     * @return Whether the action satisfies at least one condition specified by the user, @c true when no conditions
     * were specified.
     */
    [[nodiscard]] bool satisfiesConditions() const;

    /**
     * @return Whether the accumulated delta fits within the specified range.
     */
    [[nodiscard]] bool thresholdReached() const;

    bool m_blockOtherActions = false;
    qreal m_repeatInterval = 0;
    qreal m_minimumThreshold = 0;
    qreal m_maximumThreshold = 0;
    std::vector<std::shared_ptr<const Condition>> m_conditions;

    qreal m_accumulatedDelta = 0;

    /**
     * Unlike @c m_accumulatedDelta, this member contains the absolute delta and is not reset when the direction is
     * changed or subtracted from.
     */
    qreal m_absoluteAccumulatedDelta = 0;
    bool m_triggered = false;
    On m_on = On::Update;

    friend class TestGestureRecognizer;
};

}