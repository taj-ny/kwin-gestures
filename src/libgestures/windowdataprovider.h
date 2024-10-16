#pragma once

#include <QString>

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

class WindowData
{
public:
    WindowData(QString title, QString resourceClass, QString resourceName, WindowState state);

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

class WindowDataProvider
{
public:
    WindowDataProvider() = default;
    virtual ~WindowDataProvider() = default;

    virtual std::optional<const WindowData> getDataForActiveWindow() const { return std::nullopt; };
};