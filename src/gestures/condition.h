#pragma once

#include <QRegularExpression>

enum WindowState
{
    Unimportant = 0,
    Fullscreen = 1,
    Maximized = 2,
};
inline WindowState operator|(WindowState a, WindowState b)
{
    return static_cast<WindowState>(static_cast<int>(a) | static_cast<int>(b));
}

class Condition
{
public:
    Condition(bool negate, std::optional<QRegularExpression> windowClassRegex, std::optional<WindowState> windowState);

    bool isSatisfied() const;
private:
    const bool m_negate;
    const std::optional<QRegularExpression> m_windowClassRegex;
    const std::optional<WindowState> m_windowState;
};