#pragma once

#include "libgestures/actions/action.h"
#include <QRegularExpression>
#include <vector>

namespace libgestures
{

class Gesture : public QObject
{
    Q_OBJECT
public:
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
signals:
    /*
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
     * @param endedPrematurely Whether the gesture recognizer should end the gesture.
     */
    void updated(const qreal &delta, bool &endedPrematurely);
protected:
    Gesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold);

    const bool m_triggerWhenThresholdReached;
    const qreal m_threshold;
private slots:
    void onCancelled();
    void onEnded();
    void onStarted();
    void onUpdated(const qreal &delta, bool &endedPrematurely);
private:
    const uint m_minimumFingers;
    const uint m_maximumFingers;
    const bool m_triggerOneActionOnly;

    std::vector<Condition> m_conditions;

    std::vector<std::shared_ptr<GestureAction>> m_actions;

    qreal m_accumulatedDelta = 0;
    bool m_hasStarted = false;
};

} // namespace libgestures