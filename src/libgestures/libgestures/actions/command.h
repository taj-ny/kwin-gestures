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
    bool tryExecute() override;
    void setCommand(const QString &command);

private:
    std::string m_command;
};

}