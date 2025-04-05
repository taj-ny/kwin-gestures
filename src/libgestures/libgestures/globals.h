#pragma once

#include <QObject>

namespace libgestures
{
Q_NAMESPACE

enum class TriggerSpeed
{
    Any,
    Slow,
    Fast
};
Q_ENUM_NS(TriggerSpeed)

enum class TriggerType : uint32_t {
    None = 0,
    Pinch = 1u << 0,
    Press = 1u << 1,
    Rotate = 1u << 2,
    Stroke = 1u << 3,
    Swipe = 1u << 4,
    Wheel = 1u << 5,

    PinchRotate = Pinch | Rotate,
    StrokeSwipe = Stroke | Swipe,

    All = UINT32_MAX
};
Q_ENUM_NS(TriggerType)
Q_DECLARE_FLAGS(TriggerTypes, TriggerType)
Q_DECLARE_OPERATORS_FOR_FLAGS(TriggerTypes)

}