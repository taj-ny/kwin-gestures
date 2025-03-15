#include "input.h"

#include "libgestures/gestures/gesture.h"

namespace libgestures
{

Qt::KeyboardModifiers Input::keyboardModifiers() const
{
    return Qt::KeyboardModifier::NoModifier;
}

Qt::MouseButtons Input::mouseButtons() const
{
    return Qt::MouseButton::NoButton;
}

Edges Input::mouseScreenEdges(const qreal &threshold) const
{
    return Edge::None;
}

bool Input::isSendingInput() const
{
    return false;
}

std::unique_ptr<Input> Input::s_implementation = std::make_unique<Input>();
void Input::setImplementation(Input *implementation)
{
    s_implementation = std::unique_ptr<Input>(implementation);
}

}