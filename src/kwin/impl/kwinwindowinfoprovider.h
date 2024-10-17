#pragma once

#include "libgestures/windowinfoprovider.h"

class KWinWindowInfoProvider : public libgestures::WindowInfoProvider
{
public:
    KWinWindowInfoProvider() = default;

    [[nodiscard]] std::optional<const libgestures::WindowInfo> activeWindow() const override;
};