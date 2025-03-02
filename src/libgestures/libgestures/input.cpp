#include "input.h"

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

std::unique_ptr<Input> Input::s_implementation = std::make_unique<Input>();
void Input::setImplementation(Input *implementation)
{
    s_implementation = std::unique_ptr<Input>(implementation);
}

}