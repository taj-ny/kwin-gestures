#include "config/config.h"
#include "gestureseffect.h"
#include "input.h"

GesturesEffect::GesturesEffect()
    : m_inputEventFilter(new GestureInputEventFilter())
{
    KWin::input()->prependInputEventFilter(m_inputEventFilter.get());

    reconfigure(ReconfigureAll);
}

GesturesEffect::~GesturesEffect()
{
    KWin::input()->uninstallInputEventFilter(m_inputEventFilter.get());
}

void GesturesEffect::reconfigure(ReconfigureFlags flags)
{
    Config::instance().read();
}

#include "moc_gestureseffect.cpp"