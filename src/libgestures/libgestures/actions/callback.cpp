#include "callback.h"

namespace libgestures
{

bool CallbackGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    m_callback();
    return true;
}

void CallbackGestureAction::setCallback(const std::function<void()> &callback)
{
    m_callback = callback;
}

}