#pragma once

#include <QString>

namespace libgestures
{

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

class WindowInfo
{
public:
    WindowInfo(QString title, QString resourceClass, QString resourceName, WindowState state);

    QString title() const { return m_title; };
    QString resourceClass() const { return m_resourceClass; };
    QString resourceName() const { return m_resourceName; };
    WindowState state() const { return m_state; };

private:
    const QString m_title;
    const QString m_resourceClass;
    const QString m_resourceName;
    const WindowState m_state;
};

/**
 * Provides information about the currently active window.
 */
class WindowInfoProvider
{
public:
    WindowInfoProvider() = default;
    virtual ~WindowInfoProvider() = default;

    /**
     * @return The window information, or @c std::nullopt if no window is currently active.
     */
    [[nodiscard]] virtual std::optional<const WindowInfo> activeWindow() const { return std::nullopt; };
};

} // namespace libgestures