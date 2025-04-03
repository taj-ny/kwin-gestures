#include "press.h"

namespace libgestures
{

const bool &PressTrigger::instant() const
{
    return m_instant;
}

void PressTrigger::setInstant(const bool &instant)
{
    m_instant = instant;
}

}