#include "windowinfoprovider.h"

#include <utility>

namespace libgestures
{

WindowInfo::WindowInfo(QString title, QString resourceClass, QString resourceName, WindowStates state)
    : m_title(std::move(title)),
      m_resourceClass(std::move(resourceClass)),
      m_resourceName(std::move(resourceName)),
      m_state(state)
{
}

std::unique_ptr<WindowInfoProvider> WindowInfoProvider::s_implementation = std::make_unique<WindowInfoProvider>();
void WindowInfoProvider::setImplementation(WindowInfoProvider *implementation)
{
    s_implementation = std::unique_ptr<WindowInfoProvider>(implementation);
}

}