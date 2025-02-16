#include "builtingesture.h"
#include "holdgesture.h"
#include "pinchgesture.h"
#include "swipegesture.h"
#include "libgestures/conditions/callback.h"
#include "libgestures/actions/callback.h"

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

        if (animation == GestureAnimation::Window) {
            gesture->setInertiaFriction(1);
        }
    }
    gesture->setBlocksOthers(true);

    std::vector<std::unique_ptr<GestureAction>> actions;
    if (!m_actionFactories.empty()) {
        for (const auto &factory : m_actionFactories) {
            factory(actions);
        }
    } else if (m_singleAction) {
        auto action = std::make_unique<CallbackGestureAction>([this](auto &) {
            (*m_singleAction)();
        });

        if (animation == GestureAnimation::None) {
            action->setOn(instant ? On::Begin : On::End);
        } else if (m_animationFactory) {
            action->setOn(On::End);
            (*m_animationFactory)(animation, actions);
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

void BuiltinGesture::setAction(const std::function<void()> &action)
{
    m_singleAction = action;
}

void BuiltinGesture::addAction(const libgestures::On &on, const std::function<void(const qreal &progress)> &callback)
{
    m_actionFactories.push_back([on, callback](auto &actions) {
        auto action = std::make_unique<CallbackGestureAction>(callback);
        action->setOn(on);
        actions.push_back(std::move(action));
    });
}

void BuiltinGesture::addActions(const std::function<void(std::vector<std::unique_ptr<GestureAction>> &actions)> &factory)
{
    m_actionFactories.push_back(factory);
}

bool BuiltinGesture::isCompatibleWith(Gesture *gesture)
{
    return !((dynamic_cast<HoldGesture *>(gesture) && !(m_types & GestureType::Hold))
             || (dynamic_cast<PinchGesture *>(gesture) && !(m_types & GestureType::Pinch))
             || (dynamic_cast<SwipeGesture *>(gesture) && !(m_types & GestureType::Swipe)));
}

void BuiltinGesture::addGeometryAnimation(const std::function<QRectF()> &geometry, const bool &resizeDuring, const bool &keepLastFrame, const bool &fadeOut)
{
    m_animationFactory = [geometry, resizeDuring, keepLastFrame, fadeOut](const auto &type, auto &actions) {
        auto action = std::make_unique<CallbackGestureAction>([geometry, type, resizeDuring, keepLastFrame, fadeOut](const auto &) {
            AnimationHandler::implementation()->startGeometry(type, geometry(), resizeDuring, keepLastFrame, fadeOut);
        });
        action->setOn(libgestures::On::Begin);
        actions.push_back(std::move(action));

        action = std::make_unique<CallbackGestureAction>([](const auto &progress) {
            AnimationHandler::implementation()->update(progress);
        });
        action->setOn(libgestures::On::Update);
        actions.push_back(std::move(action));

        action = std::make_unique<CallbackGestureAction>([](const auto &progress) {
            AnimationHandler::implementation()->end();
        });
        action->setOn(libgestures::On::End);
        actions.push_back(std::move(action));

        action = std::make_unique<CallbackGestureAction>([](const auto &progress) {
            AnimationHandler::implementation()->cancel();
        });
        action->setOn(libgestures::On::Cancel);
        actions.push_back(std::move(action));
    };
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
qreal BuiltinGesture::s_defaultMaxSwipeDelta = 300;
qreal BuiltinGesture::s_defaultMinPinchDelta = 0.4;
qreal BuiltinGesture::s_defaultMaxPinchDelta = 0.7;

}