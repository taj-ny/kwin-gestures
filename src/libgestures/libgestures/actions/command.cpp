#include "command.h"

namespace libgestures
{

bool CommandGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    std::ignore = std::system(m_command.c_str());
    return true;
}

void CommandGestureAction::setCommand(const QString &command)
{
    m_command = (command + " &").toStdString();
}

}