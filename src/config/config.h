#pragma once

#include "gestures/gesture.h"
#include <KConfigGroup>

class Config
{
public:
    static Config &instance()
    {
        static Config instance;
        return instance;
    }

    ~Config() = default;

    std::vector<std::shared_ptr<Gesture>> gestures;

    void read();

private:
    Config() = default;

    static std::vector<Condition> readConditions(const KConfigGroup &group);
    static std::vector<int> stringIntListToSortedIntVector(const QList<QString> &list);
};