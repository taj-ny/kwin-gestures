#include "windowdataprovider.h"

WindowData::WindowData(QString title, QString resourceClass, QString resourceName, WindowState state)
    : m_title(title),
      m_resourceClass(resourceClass),
      m_resourceName(resourceName),
      m_state(state)
{
}