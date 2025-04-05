#pragma once

#include "libgestures/conditions/callbackcondition.h"

#include <memory>

namespace libgestures
{

std::shared_ptr<Condition> makeCondition(const bool &result)
{
    return std::make_shared<CallbackCondition>([result]() { return result; });
}

}
