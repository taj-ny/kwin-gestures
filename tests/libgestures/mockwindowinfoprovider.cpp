#include "mockwindowinfoprovider.h"

#include <utility>

namespace libgestures
{

MockWindowInfoProvider::MockWindowInfoProvider(std::optional<const WindowInfo> info)
    : m_info(std::move(info))
{
}

std::optional<const WindowInfo> MockWindowInfoProvider::activeWindow() const
{
    return m_info;
}

} // namespace libgestures