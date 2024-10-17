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

    void setTriggerWhenThresholdReached(const bool &triggerWhenThresholdReached);
    void setThreshold(const qreal &threshold);
    void setFingers(const uint8_t &minimum, const uint8_t &maximum);
    void setTriggerOneActionOnly(const bool &triggerOneActionOnly);
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

    std::vector<Condition> m_conditions;

    std::vector<std::shared_ptr<GestureAction>> m_actions;

    qreal m_accumulatedDelta = 0;
    bool m_hasStarted = false;
};

} // namespace libgestures