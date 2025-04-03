#pragma once

#include "motion.h"

namespace libgestures
{

enum class PinchDirection : uint32_t
{
    In = 1u << 0,
    Out = 1u << 1,
    Any = UINT32_MAX
};
enum class RotateDirection : uint32_t
{
    Clockwise = 1u << 0,
    Counterclockwise = 1u << 1,
    Any = UINT32_MAX
};
enum class SwipeDirection : uint32_t
{
    Left = 1u << 0,
    Right = 1u << 1,
    Up = 1u << 2,
    Down = 1u << 3,
    LeftRight = Left | Right,
    UpDown = Up | Down,
    Any = UINT32_MAX
};

class DirectionalMotionTriggerUpdateEvent : public MotionTriggerUpdateEvent
{
public:
    DirectionalMotionTriggerUpdateEvent() = default;

    const uint32_t &direction() const;
    void setDirection(const uint32_t &direction);

private:
    uint32_t m_direction = UINT32_MAX;
};

/**
 * An input action that involves directional motion.
 */
class DirectionalMotionTrigger : public MotionTrigger
{
public:
    /**
     * @return Whether the direction matches.
     * @see MotionTrigger::canUpdate
     */
    bool canUpdate(const TriggerUpdateEvent *event) const override;

private:
    uint32_t m_direction = 0;
};

}