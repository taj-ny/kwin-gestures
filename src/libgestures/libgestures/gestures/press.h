#pragma once

#include "gesture.h"

namespace libgestures
{

class PressGesture : public Gesture
{
public:
    const bool &instant() const;
    void setInstant(const bool &instant);

private:
    bool m_instant = false;
};

}