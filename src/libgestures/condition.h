#pragma once

#include <QRegularExpression>
#include "windowdataprovider.h"


class Condition
{
public:
    Condition(std::shared_ptr<WindowDataProvider> windowDataProvider, bool negate, std::optional<QRegularExpression> windowClassRegex, std::optional<WindowState> windowState);

    bool isSatisfied() const;
private:
    const std::shared_ptr<WindowDataProvider> m_windowDataProvider;
    const bool m_negate;
    const std::optional<QRegularExpression> m_windowClassRegex;
    const std::optional<WindowState> m_windowState;
};