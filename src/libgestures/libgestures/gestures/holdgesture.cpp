#include "holdgesture.h"

namespace libgestures
{

HoldGesture::HoldGesture(bool triggerWhenThresholdReached, uint minimumFingers, uint maximumFingers, bool triggerOneActionOnly, qreal threshold)
    : Gesture(triggerWhenThresholdReached, minimumFingers, maximumFingers, triggerOneActionOnly, threshold)
{
}

} // namespace libgestures