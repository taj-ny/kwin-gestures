#pragma once

#include <QString>

class GlobalShortcutInvoker
{
public:
    GlobalShortcutInvoker() = default;
    virtual ~GlobalShortcutInvoker() = default;

    virtual void invoke(const QString &component, const QString &shortcut) const { };
};