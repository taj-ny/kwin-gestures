#pragma once

#include "actions/action.h"
#include "condition.h"
#include <QRegularExpression>
#include <vector>

class Gesture : public QObject
{
    Q_OBJECT
public:
    /*
     * Called when the gesture has been cancelled.
     */
    virtual void cancelled();

    /**
     * Called when the gesture has ended.
     */
    virtual void ended();

    /**
     * Called when the gesture has started and reached the specified threshold. If no threshold has been specified, the
     * method will be called immediately.
     */
    virtual void start();

    /**
     * Called when the gesture has been updated.
     *
     * @returns Whether the gesture recognizer should end the gesture.
     */
    virtual bool update(const qreal &delta);

    /**
     * @returns Whether the amount of fingers fits within the range specified by the user and all (or none if none
     * specified) custom conditions are satisfied.
     */
    bool satisfiesConditions(const uint8_t &fingerCount) const;

    /**
     * @returns Whether the threshold has been reached.
     */
    virtual bool thresholdReached(const qreal &accumulatedDelta) const;

    void addAction(std::shared_ptr<GestureAction> action);
    void addCondition(const Condition &condition);
protected:
    Gesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold);

    const bool m_triggerWhenThresholdReached;
    const qreal m_threshold;
private:
    const uint m_minimumFingers;
    const uint m_maximumFingers;
    const bool m_triggerOneActionOnly;

    std::vector<Condition> m_conditions;

    std::vector<std::shared_ptr<GestureAction>> m_actions;

    qreal m_accumulatedDelta = 0;
    bool m_hasStarted = false;
};