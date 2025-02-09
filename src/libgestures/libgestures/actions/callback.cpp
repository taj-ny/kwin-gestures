#include "callback.h"

namespace libgestures
{

CallbackGestureAction::CallbackGestureAction(const std::function<void(const qreal &progress)> &callback)
    : m_callback(callback)
{
}

bool CallbackGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    m_callback(m_currentProgress);
    return true;
}

}