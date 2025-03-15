#include "press.h"

namespace libgestures
{

const bool &PressGesture::instant() const
{
    return m_instant;
}

void PressGesture::setInstant(const bool &instant)
{
    m_instant = instant;
}

}