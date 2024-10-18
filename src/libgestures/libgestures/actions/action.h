#pragma once

#include "libgestures/condition.h"

namespace libgestures
{

enum When
{
    Cancelled,
    Ended,
    Started,
    Updated
};

class GestureAction : public QObject
{
    Q_OBJECT
public:
    GestureAction();

    virtual void execute();
    bool canExecute() const;

    bool repeat() const { return m_repeatInterval != 0; };

    /**
     * @return Whether any other actions belonging to a gesture should not be executed. @c true if the action has been
     * executed and blockOtherActions has been set to @c true, @c false otherwise.
     */
    [[nodiscard]] bool blocksOtherActions() const;

    void addCondition(const std::shared_ptr<const Condition> &condition);

    /**
     * @returns Whether the action satisfies at least one condition specified by the user, @c true when no conditions
     * were specified.
     */
    bool satisfiesConditions() const;

    void setBlockOtherActions(const bool &blockOtherActions);
    void setRepeatInterval(const qreal &interval);
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
    void gestureUpdated(const qreal &delta);
private slots:
    void onGestureCancelled();
    void onGestureEnded();
    void onGestureStarted();
    void onGestureUpdated(const qreal &delta);
private:
    bool m_blockOtherActions = false;
    qreal m_repeatInterval = 0;
    std::vector<std::shared_ptr<const Condition>> m_conditions;

    qreal m_accumulatedDelta = 0;
    bool m_triggered = false;
    When m_when = When::Updated;

    friend class TestGestureRecognizer;
};

} // namespace libgestures