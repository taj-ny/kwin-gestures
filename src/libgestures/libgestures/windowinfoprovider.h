#pragma once

#include <QString>
#include "yaml-cpp/yaml.h"

namespace libgestures
{

enum WindowState
{
    Maximized = 1u << 0,
    Fullscreen = 1u << 1,
    All = 0u - 1
};
Q_DECLARE_FLAGS(WindowStates, WindowState)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowStates)

class WindowInfoProvider;

class WindowInfo
{
public:
    WindowInfo(QString title, QString resourceClass, QString resourceName, WindowStates state);

    QString title() const { return m_title; };
    QString resourceClass() const { return m_resourceClass; };
    QString resourceName() const { return m_resourceName; };
    WindowStates state() const { return m_state; };

private:
    const QString m_title;
    const QString m_resourceClass;
    const QString m_resourceName;
    const WindowStates m_state;
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
    [[nodiscard]] virtual std::optional<const WindowInfo> activeWindow() const
    {
        return std::nullopt;
    };

    static WindowInfoProvider *implementation()
    {
        return s_implementation.get();
    }
    static void setImplementation(WindowInfoProvider *implementation);

private:
    static std::unique_ptr<WindowInfoProvider> s_implementation;
};

}