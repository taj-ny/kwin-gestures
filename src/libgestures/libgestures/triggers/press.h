#pragma once

#include "trigger.h"

namespace libgestures
{

class PressTrigger : public Trigger
{
public:
    const bool &instant() const;
    void setInstant(const bool &instant);

private:
    bool m_instant = false;
};

}