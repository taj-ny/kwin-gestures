#include "libgestures/windowinfoprovider.h"

namespace libgestures
{

class MockWindowInfoProvider : public WindowInfoProvider
{
public:
    explicit MockWindowInfoProvider(std::optional<const WindowInfo> info);

    [[nodiscard]] std::optional<const WindowInfo> activeWindow() const override;
private:
    const std::optional<const WindowInfo> m_info;
};

}