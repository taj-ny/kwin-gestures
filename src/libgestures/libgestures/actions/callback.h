#pragma once

#include "action.h"

namespace libgestures
{

/**
 * Calls a function.
 */
class CallbackGestureAction : public GestureAction
{
public:
    CallbackGestureAction(const std::function<void(const qreal &progress)> &callback);
    bool tryExecute() override;

private:
    const std::function<void(const qreal &progress)> m_callback;
};

}