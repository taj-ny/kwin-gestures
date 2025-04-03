#pragma once

#include "libgestures/triggers/directionalmotion.h"
#include "triggerhandler.h"

Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_HANDLER_MOTION)

namespace libgestures
{

enum class Axis
{
    Horizontal,
    Vertical,
    None
};

class MotionTriggerHandler : public TriggerHandler
{
public:
    MotionTriggerHandler();

    bool updateMotion(const QPointF &delta);

protected:
    bool determineSpeed(const qreal &delta, const qreal &fastThreshold);

    void triggerActivating(Trigger *trigger) override;
    void reset() override;

    TriggerSpeed m_speed = TriggerSpeed::Any;

private:
    void strokeGestureEnder(const TriggerEndEvent *event);

    Axis m_currentSwipeAxis = Axis::None;
    QPointF m_currentSwipeDelta;
    qreal m_swipeDeltaMultiplier = 1.0;

    bool m_isDeterminingSpeed = false;
    uint8_t m_inputEventsToSample = 3;
    uint8_t m_sampledInputEvents = 0;
    qreal m_accumulatedAbsoluteSampledDelta = 0;

    std::vector<QPointF> m_stroke;
};

}