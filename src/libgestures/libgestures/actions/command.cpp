#include "command.h"

#include <thread>

namespace libgestures
{

void CommandGestureAction::execute()
{
    std::thread thread([this]() {
        std::system(m_command.c_str());
    });
    thread.detach();
}

void CommandGestureAction::setCommand(const QString &command)
{
    m_command = command.toStdString();
}

}