#include "command.h"

#include <thread>

namespace libgestures
{

bool CommandGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    std::thread thread([this]() {
        std::system(m_command.c_str());
    });
    thread.detach();
    return true;
}

void CommandGestureAction::setCommand(const QString &command)
{
    m_command = command.toStdString();
}

}