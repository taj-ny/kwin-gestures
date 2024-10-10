#include "gestures.h"

namespace KWin
{

SwipeGesture::SwipeGesture(SwipeDirection direction, int minimumFingerCount, int maximumFingerCount)
    : m_direction(direction), m_minimumFingerCount(minimumFingerCount), m_maximumFingerCount(maximumFingerCount)
{
}

}