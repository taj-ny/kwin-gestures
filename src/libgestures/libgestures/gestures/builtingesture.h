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

enum class GestureAnimation {
    None,
    Overlay
};

class BuiltinGesture
{
public:
    /**
     * Assigns this built-in gesture to the specified gesture.
     */
    void assignTo(Gesture *gesture, const GestureAnimation &animation, const bool &instant) const;

    void addRequiredCondition(const std::function<bool()> &callback);

    /**
     * @return Whether this built-in gesture can be assigned to the specified gesture.
     */
    bool isCompatibleWith(Gesture *gesture);

    void setOneToOneActionFactory(std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)> factory);
    void setAnimationFactory(const GestureAnimation &type, std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)> factory);
    void setSingleActionFactory(std::function<std::unique_ptr<GestureAction>()> factory);

    /**
     * @param types The gestures this action can be assigned to.
     */
    void setCompatibleGestureTypes(const GestureTypes &types);

    static void setDefaultMinimumSwipeDelta(const qreal &delta);
    static void setDefaultMaximumSwipeDelta(const qreal &delta);
    static void setDefaultMinimumPinchDelta(const qreal &delta);
    static void setDefaultMaximumPinchDelta(const qreal &delta);

    static void registerGesture(const QString &id, std::unique_ptr<BuiltinGesture> gesture);

    static const std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> &registeredGestures();

private:
    std::unordered_map<GestureAnimation, std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)>> m_animationFactories;
    std::optional<std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)>> m_oneToOneActionFactory;
    std::optional<std::function<std::unique_ptr<GestureAction>()>> m_singleActionFactory;
    GestureTypes m_types = GestureType::All;
    std::vector<std::shared_ptr<Condition>> m_conditions;

    static std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> s_registeredGestures;

    static qreal s_defaultMinSwipeDelta;
    static qreal s_defaultMaxSwipeDelta;
    static qreal s_defaultMinPinchDelta;
    static qreal s_defaultMaxPinchDelta;
};

}