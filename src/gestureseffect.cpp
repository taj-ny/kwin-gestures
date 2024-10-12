#include "config/config.h"
#include "gestureseffect.h"
#include "input.h"

GesturesEffect::GesturesEffect()
    : m_inputEventFilter(new GestureInputEventFilter())
{
#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_inputEventFilter.get());
#else
    // In KWin < 6.2 you can only add a filter at the beginning or at the end. The filter is placed before a bunch of
    // other filters, so this may cause some issues. The filter has to make sure the screen is not locked.
    // https://invent.kde.org/plasma/kwin/-/blob/v6.2.0/src/input.h#L364
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
    Q_UNUSED(flags)

    Config::instance().read();
}