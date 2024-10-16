#pragma once

#include "globalshortcutinvoker.h"

class KDEGlobalShortcutInvoker : public GlobalShortcutInvoker
{
public:
    KDEGlobalShortcutInvoker() = default;

    void invoke(const QString &component, const QString &shortcut) const override;
};