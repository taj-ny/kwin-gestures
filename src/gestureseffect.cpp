#include "config/config.h"
#include "gestureseffect.h"
#include "input.h"

GesturesEffect::GesturesEffect()
    : m_inputEventFilter(new GestureInputEventFilter())
{
#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_inputEventFilter.get());
#else
    KWin::input()->prependInputEventFilter(m_inputEventFilter.get());
#endif

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