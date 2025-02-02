#pragma once

#include "libgestures/windowinfoprovider.h"

class KWinWindowInfoProvider : public libgestures::WindowInfoProvider
{
public:
    KWinWindowInfoProvider() = default;

    std::optional<const libgestures::WindowInfo> activeWindow() const override;
};