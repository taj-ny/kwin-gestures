#pragma once

#include "gestures/gesture.h"

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
};