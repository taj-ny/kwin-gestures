#pragma once

#include "gesture.h"

#include "libgestures/actions/action.h"
#include "libgestures/animations/handler.h"

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

class BuiltinGesture
{
public:
    BuiltinGesture() = default;

    /**
     * Assigns this built-in gesture to the specified gesture.
     */
    void assignTo(Gesture *gesture, const GestureAnimation &animation, const bool &instant) const;

    void addRequiredCondition(const std::function<bool()> &condition);

    /**
     * @return Whether this built-in gesture can be assigned to the specified gesture.
     */
    bool isCompatibleWith(Gesture *gesture);

    /**
     * Sets the action to execute at the end of this gesture. It will also be executed after the animation, if set.
     * For more complex gestures use @c setActionFactory.
     */
    void setAction(const std::function<void()> &action);
    void addAction(const libgestures::On &on, const std::function<void(const qreal &progress)> &action);
    void addActions(const std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)> &factory);


    /**
     * @param geometry Executed right before the animation starts.
     */
    void addGeometryAnimation(const std::function<QRectF()> &geometry, const bool &resizeDuring = false, const bool &keepLastFrame = false, const bool &fadeOut = false);

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
    std::optional<std::function<void()>> m_singleAction;
    std::vector<std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)>> m_actionFactories;
    std::optional<std::function<void(const GestureAnimation &type, std::vector<std::unique_ptr<GestureAction>> &actions)>> m_animationFactory;

    GestureTypes m_types = GestureType::All;
    std::vector<std::shared_ptr<Condition>> m_conditions;

    static std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> s_registeredGestures;

    static qreal s_defaultMinSwipeDelta;
    static qreal s_defaultMaxSwipeDelta;
    static qreal s_defaultMinPinchDelta;
    static qreal s_defaultMaxPinchDelta;
};

}