#include "input.h"

namespace libgestures
{

std::unique_ptr<Input> Input::s_implementation = std::make_unique<Input>();
void Input::setImplementation(Input *implementation)
{
    s_implementation = std::unique_ptr<Input>(implementation);
}

}