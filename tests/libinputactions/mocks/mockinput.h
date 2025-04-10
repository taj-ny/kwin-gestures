#pragma once

#include <libinputactions/input.h>

#include <gmock/gmock.h>

namespace libinputactions
{

class MockInput : public Input
{
public:
    MockInput() = default;
};

}