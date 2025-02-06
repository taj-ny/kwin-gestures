#include "builtingesture.h"
#include "holdgesture.h"
#include "pinchgesture.h"
#include "swipegesture.h"

#include <vector>

namespace libgestures
{

void BuiltinGesture::assignTo(Gesture *gesture, const GestureConfiguration &config) const
{
    m_assigner(gesture, config);
}

bool BuiltinGesture::isCompatibleWith(Gesture *gesture)
{
    return !((dynamic_cast<HoldGesture *>(gesture) && !(m_types & GestureType::Hold))
             || (dynamic_cast<PinchGesture *>(gesture) && !(m_types & GestureType::Pinch))
             || (dynamic_cast<SwipeGesture *>(gesture) && !(m_types & GestureType::Swipe)));
}

BuiltinGesture &BuiltinGesture::setAssigner(std::function<void(Gesture *, const GestureConfiguration &)> assigner)
{
    m_assigner = assigner;
    return *this;
}

BuiltinGesture &BuiltinGesture::setCompatibleGestureTypes(const GestureTypes &types)
{
    m_types = types;
    return *this;
}

void BuiltinGesture::registerGesture(const QString &id, std::unique_ptr<BuiltinGesture> gesture)
{
    s_registeredGestures[id] = std::move(gesture);
}

const std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> &BuiltinGesture::registeredGestures()
{
    return s_registeredGestures;
}

std::unordered_map<QString, std::unique_ptr<BuiltinGesture>> BuiltinGesture::s_registeredGestures;

}