#pragma once

#include "swipe.h"

namespace libgestures
{

class WheelGesture : public SwipeGesture
{
public:
    WheelGesture();

    GestureType type() const override;
};

}