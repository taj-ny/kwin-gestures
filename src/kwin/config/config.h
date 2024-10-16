#pragma once

#include "condition.h"
#include "gestures.h"
#include "globalshortcutinvoker.h"
#include "inputfilter.h"
#include <KConfigGroup>
#include "virtualinputdevice.h"
#include "windowdataprovider.h"

class Config
{
public:
    static Config &instance()
    {
        static Config instance;
        return instance;
    }

    ~Config() = default;

    void read(std::shared_ptr<GestureInputEventFilter> filter, std::shared_ptr<GlobalShortcutInvoker> globalShortcutInvoker, std::shared_ptr<VirtualInputDevice> virtualInputDevice, std::shared_ptr<WindowDataProvider> windowDataProvider);

private:
    Config() = default;

    static std::vector<Condition> readConditions(const KConfigGroup &group, std::shared_ptr<WindowDataProvider> windowDataProvider);
    static std::vector<int> stringIntListToSortedIntVector(const QList<QString> &list);
};