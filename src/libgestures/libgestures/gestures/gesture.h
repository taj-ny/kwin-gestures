#pragma once

#include "libgestures/actions/action.h"
#include <QRegularExpression>
#include <vector>

namespace libgestures
{

enum GestureSpeed {
    Any,
    Slow,
    Fast
};

class Gesture : public QObject
{
    Q_OBJECT
public:
    Gesture();

    /**
     * @returns Whether the amount of fingers fits within the specified range, all (if any) conditions are
     * satisfied, and there is at least one action (if any) that satisfies conditions.
     */
    bool satisfiesConditions(const uint8_t &fingerCount) const;

    GestureSpeed speed() const;

    void addAction(const std::shared_ptr<GestureAction> &action);
    void addCondition(const std::shared_ptr<const Condition> &condition);

    void setSpeed(const GestureSpeed &speed);
    void setThresholds(const qreal &minimum, const qreal &maximum);
    void setFingers(const uint8_t &minimum, const uint8_t &maximum);
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
    void updated(const qreal &delta, const QPointF &deltaPointMultiplied, bool &endedPrematurely);
private slots:
    void onCancelled();
    void onEnded();
    void onStarted();
    void onUpdated(const qreal &delta, const QPointF &deltaPointMultiplied, bool &endedPrematurely);

private:
    /**
     * @return Whether the accumulated delta fits within the specified range.
     */
    bool thresholdReached() const;

    uint8_t m_minimumFingers = 0;
    uint8_t m_maximumFingers = 0;
    qreal m_minimumThreshold = 0;
    qreal m_maximumThreshold = 0;
    GestureSpeed m_speed = GestureSpeed::Any;

    std::vector<std::shared_ptr<const Condition>> m_conditions;

    std::vector<std::shared_ptr<GestureAction>> m_actions;

    qreal m_absoluteAccumulatedDelta = 0;
    bool m_hasStarted = false;
};

}