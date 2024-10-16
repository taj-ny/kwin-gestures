#pragma once

#include "windowdataprovider.h"

class KWinWindowDataProvider : public WindowDataProvider
{
public:
    KWinWindowDataProvider() = default;

    std::optional<const WindowData> getDataForActiveWindow() const override;
};