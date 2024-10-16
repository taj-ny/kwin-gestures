#include "mockwindowdataprovider.h"

#include <utility>

MockWindowDataProvider::MockWindowDataProvider(std::optional<const WindowData> data)
    : m_data(std::move(data))
{
}

std::optional<const WindowData> MockWindowDataProvider::getDataForActiveWindow() const
{
    return m_data;
}