#include "actioncollection.h"

#include <vector>

namespace libgestures
{

std::vector<std::unique_ptr<GestureAction>> ActionCollection::actions(const GestureConfiguration &config) const
{
    std::vector<std::unique_ptr<GestureAction>> actions;
    m_actionFactory(actions, config);
    return actions;
}

void ActionCollection::setActionFactory(std::function<void(std::vector<std::unique_ptr<GestureAction>> &, const GestureConfiguration &)> factory)
{
    m_actionFactory = factory;
}

const GestureTypes &ActionCollection::gestureTypes() const
{
    return m_types;
}

void ActionCollection::setGestureTypes(const GestureTypes &types)
{
    m_types = types;
}

void ActionCollection::registerCollection(const QString &name, std::unique_ptr<ActionCollection> collection)
{
    s_collections[name] = std::move(collection);
}

const std::unordered_map<QString, std::unique_ptr<ActionCollection>> &ActionCollection::registeredCollections()
{
    return s_collections;
}

std::unordered_map<QString, std::unique_ptr<ActionCollection>> ActionCollection::s_collections;

}