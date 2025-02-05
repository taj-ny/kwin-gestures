#pragma once

#include "libgestures/libgestures/actions/action.h"

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

/**
 * A built-in collection of actions, assignable to gestures.
 */
class ActionCollection
{
public:
    /**
     * @return A vector of copied actions.
     */
    std::vector<std::unique_ptr<GestureAction>> actions(const GestureConfiguration &config = GestureConfiguration()) const;
    void setActionFactory(std::function<void(std::vector<std::unique_ptr<GestureAction>> &, const GestureConfiguration &)> factory);

    /**
     * @return The gestures this action can be assigned to.
     */
    const GestureTypes &gestureTypes() const;
    void setGestureTypes(const GestureTypes &types);

    static void registerCollection(const QString &id, std::unique_ptr<ActionCollection> collection);
    static const std::unordered_map<QString, std::unique_ptr<ActionCollection>> &registeredCollections();

private:
    std::vector<std::unique_ptr<GestureAction>> m_actions;
    std::function<void(std::vector<std::unique_ptr<GestureAction>> &, const GestureConfiguration &)> m_actionFactory;

    GestureTypes m_types = GestureType::All;

    static std::unordered_map<QString, std::unique_ptr<ActionCollection>> s_collections;
};

}