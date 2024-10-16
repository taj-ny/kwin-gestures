#include "windowdataprovider.h"

class MockWindowDataProvider : public WindowDataProvider
{
public:
    MockWindowDataProvider(std::optional<const WindowData> data);

    std::optional<const WindowData> getDataForActiveWindow() const override;
private:
    const std::optional<const WindowData> m_data;
};