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
    Gesture();

    /**
     * @returns Whether the amount of fingers fits within the specified range, all (if any) conditions are
     * satisfied, and there is at least one action (if any) that satisfies conditions.
     */
    [[nodiscard]] bool satisfiesConditions(const uint8_t &fingerCount) const;

    /**
     * @returns Whether the specified threshold has been reached.
     */
    [[nodiscard]] virtual bool thresholdReached(const qreal &accumulatedDelta) const;

    void addAction(const std::shared_ptr<GestureAction> &action);
    void addCondition(const std::shared_ptr<const Condition> &condition);

    /**
     * @param triggerWhenThresholdReached Whether gesture actions should be immediately triggered once the specified
     * threshold is reached.
     */
    void setTriggerWhenThresholdReached(const bool &triggerWhenThresholdReached);
    void setThreshold(const qreal &threshold);
    void setFingers(const uint8_t &minimum, const uint8_t &maximum);

    /**
     * @param triggerOneActionOnly Whether only one action should be executed during a gesture. This can cause a
     * gesture to end prematurely.
     */
    void setTriggerOneActionOnly(const bool &triggerOneActionOnly);
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
    void updated(const qreal &delta, bool &endedPrematurely);
protected:
    bool m_triggerWhenThresholdReached = false;
    qreal m_threshold = 0;
private slots:
    void onCancelled();
    void onEnded();
    void onStarted();
    void onUpdated(const qreal &delta, bool &endedPrematurely);
private:
    uint8_t m_minimumFingers = 0;
    uint8_t m_maximumFingers = 0;
    bool m_triggerOneActionOnly = false;

    std::vector<std::shared_ptr<const Condition>> m_conditions;

    std::vector<std::shared_ptr<GestureAction>> m_actions;

    qreal m_accumulatedDelta = 0;
    bool m_hasStarted = false;
};

} // namespace libgestures