#pragma once

#include "action.h"

#include <QString>

namespace libgestures
{

/**
 * Runs a command.
 */
class CommandGestureAction : public GestureAction
{
public:
    CommandGestureAction(const QString &command);
    bool tryExecute() override;

private:
    const std::string m_command;
};

}