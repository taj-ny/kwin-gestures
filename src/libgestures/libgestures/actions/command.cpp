#include "command.h"

namespace libgestures
{

CommandGestureAction::CommandGestureAction(const QString &command)
    : m_command((command + " &").toStdString())
{
}

bool CommandGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    std::ignore = std::system(m_command.c_str());
    return true;
}

}