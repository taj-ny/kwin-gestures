#pragma once

#include "inputfilter.h"
#include <KConfigGroup>
#include "libgestures/condition.h"
#include "libgestures/input.h"

class Config
{
public:
    static Config &instance()
    {
        static Config instance;
        return instance;
    }

    ~Config() = default;

    void read(std::shared_ptr<GestureInputEventFilter> filter, std::shared_ptr<libgestures::Input> input, std::shared_ptr<libgestures::WindowInfoProvider> windowInfoProvider);

private:
    Config() = default;

    static std::vector<std::shared_ptr<libgestures::Condition>> readConditions(const KConfigGroup &group, std::shared_ptr<libgestures::WindowInfoProvider> windowInfoProvider);
    static std::vector<int> stringIntListToSortedIntVector(const QList<QString> &list);
};