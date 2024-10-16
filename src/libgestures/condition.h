#pragma once

#include <QRegularExpression>
#include "windowdataprovider.h"

class Condition
{
public:
    explicit Condition(std::shared_ptr<WindowDataProvider> windowDataProvider);

    bool isSatisfied() const;

    void setNegate(const bool &negate);
    void setWindowClassRegex(const QRegularExpression &windowClassRegex);
    void setWindowState(const WindowState &windowState);
private:
    bool isWindowClassRegexSubConditionSatisfied(const WindowData &data) const;
    bool isWindowStateSubConditionSatisfied(const WindowData &data) const;

    const std::shared_ptr<WindowDataProvider> m_windowDataProvider;
    bool m_negate = false;
    std::optional<QRegularExpression> m_windowClassRegex = std::nullopt;
    std::optional<WindowState> m_windowState = std::nullopt;

    friend class TestCondition;
};