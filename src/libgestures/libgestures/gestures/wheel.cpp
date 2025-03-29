#include "wheel.h"

namespace libgestures
{

WheelGesture::WheelGesture()
{
    setFingerCountIsRelevant(false);
}

GestureType WheelGesture::type() const
{
    return GestureType::Wheel;
}

}