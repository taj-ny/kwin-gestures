#pragma once

#include "action.h"

namespace libgestures
{

// TODO This action should have access to the gesture delta and possibly some more things
/**
 * Calls a function.
 */
class CallbackGestureAction : public GestureAction
{
public:
    bool tryExecute() override;
    CallbackGestureAction &setCallback(const std::function<void()> &callback);

private:
    std::function<void()> m_callback;
};

}