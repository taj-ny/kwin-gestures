#include "builtingesture.h"
#include "holdgesture.h"
#include "pinchgesture.h"
#include "swipegesture.h"
#include "libgestures/conditions/callback.h"

#include <vector>

namespace libgestures
{

void BuiltinGesture::assignTo(Gesture *gesture, const GestureAnimation &animation, const bool &instant) const
{
    if (animation != GestureAnimation::None) {
        if (dynamic_cast<PinchGesture *>(gesture)) {
            gesture->setMinimumDelta(s_defaultMinPinchDelta);
            gesture->setMaximumDelta(s_defaultMaxPinchDelta);
        } else if (dynamic_cast<SwipeGesture *>(gesture)) {
            gesture->setMinimumDelta(s_defaultMinSwipeDelta);
            gesture->setMaximumDelta(s_defaultMaxSwipeDelta);
        }
    }
    gesture->setBlocksOthers(true);

    std::vector<std::unique_ptr<GestureAction>> actions;
    if (m_oneToOneActionFactory) {
        (*m_oneToOneActionFactory)(actions);
    } else if (m_singleActionFactory) {
        auto action = (*m_singleActionFactory)();
        if (m_animationFactories.contains(GestureAnimation::Overlay) && animation == GestureAnimation::Overlay) {
            action->setOn(On::End);
            m_animationFactories.at(GestureAnimation::Overlay)(actions);
        }
        if (animation == GestureAnimation::None) {
            action->setOn(instant ? On::Begin : On::End);
        }
        gesture->addAction(std::move(action));
    }
    for (auto &action : actions) {
        gesture->addAction(std::move(action));
    }

    for (auto &condition : m_conditions) {
        gesture->addCondition(condition);
    }
}

void BuiltinGesture::addRequiredCondition(const std::function<bool()> &callback)
{
    auto condition = std::make_shared<CallbackCondition>(callback);
    condition->setRequired(true);
    m_conditions.push_back(condition);
}

void BuiltinGesture::setAnimationFactory(const GestureAnimation &type, std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)> factory)
{
    m_animationFactories[type] = factory;
}

void BuiltinGesture::setOneToOneActionFactory(std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)> factory)
{
    m_oneToOneActionFactory = factory;
}

void BuiltinGesture::setSingleActionFactory(std::function<std::unique_ptr<GestureAction>()> factory)
{
    m_singleActionFactory = factory;
}

bool BuiltinGesture::isCompatibleWith(Gesture *gesture)
{
    return !((dynamic_cast<HoldGesture *>(gesture) && !(m_types & GestureType::Hold))
             || (dynamic_cast<PinchGesture *>(gesture) && !(m_types & GestureType::Pinch))
             || (dynamic_cast<SwipeGesture *>(gesture) && !(m_types & GestureType::Swipe)));
}


void BuiltinGesture::setCompatibleGestureTypes(const GestureTypes &types)
{
    m_types = types;
}

void BuiltinGesture::registerGesture(const QString &id, std::unique_ptr<BuiltinGesture> gesture)
{
    s_registeredGestures[id] = std::move(gesture);
}

const std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> &BuiltinGesture::registeredGestures()
{
    return s_registeredGestures;
}

void BuiltinGesture::setDefaultMinimumSwipeDelta(const qreal &delta)
{
    s_defaultMinSwipeDelta = delta;
}

void BuiltinGesture::setDefaultMaximumSwipeDelta(const qreal &delta)
{
    s_defaultMaxSwipeDelta = delta;
}

void BuiltinGesture::setDefaultMinimumPinchDelta(const qreal &delta)
{
    s_defaultMinPinchDelta = delta;
}

void BuiltinGesture::setDefaultMaximumPinchDelta(const qreal &delta)
{
    s_defaultMaxPinchDelta = true;
}

std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> BuiltinGesture::s_registeredGestures;
qreal BuiltinGesture::s_defaultMinSwipeDelta = 50;
qreal BuiltinGesture::s_defaultMaxSwipeDelta = 200;
qreal BuiltinGesture::s_defaultMinPinchDelta = 0.25;
qreal BuiltinGesture::s_defaultMaxPinchDelta = 0.5;

}