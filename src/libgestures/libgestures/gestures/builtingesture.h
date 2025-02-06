#pragma once

#include "libgestures/libgestures/actions/action.h"
#include "gesture.h"

namespace libgestures
{

enum class GestureType : uint32_t {
    Swipe = 1u << 0,
    Hold = 1u << 1,
    Pinch = 1u << 2,
    All = 0u - 1
};
Q_DECLARE_FLAGS(GestureTypes, GestureType)
Q_DECLARE_OPERATORS_FOR_FLAGS(GestureTypes)

struct GestureConfiguration
{
    bool isInstant = false;
};

class BuiltinGesture
{
public:
    /**
     * Assigns this built-in gesture to the specified gesture.
     */
    void assignTo(Gesture *gesture, const GestureConfiguration &config = GestureConfiguration()) const;

    /**
     * @return Whether this built-in gesture can be assigned to the specified gesture.
     */
    bool isCompatibleWith(Gesture *gesture);

    /**
     * @param assigner The function that will assign the built-in gesture.
     */
    void setAssigner(std::function<void(Gesture *, const GestureConfiguration &)> assigner);

    /**
     * @param types The gestures this action can be assigned to.
     */
    void setCompatibleGestureTypes(const GestureTypes &types);

    static void registerGesture(const QString &id, std::unique_ptr<BuiltinGesture> gesture);
    static const std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> &registeredGestures();

private:
    std::function<void(Gesture *, const GestureConfiguration &)> m_assigner;

    GestureTypes m_types = GestureType::All;

    static std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> s_registeredGestures;
};

}