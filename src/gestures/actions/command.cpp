#include "command.h"

CommandGestureAction::CommandGestureAction(QString command)
    : command(std::move(command))
{
}

void CommandGestureAction::execute()
{
    std::system((command + " &").toStdString().c_str());
}