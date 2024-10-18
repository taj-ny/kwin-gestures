#include "config/config.h"
#include "effect.h"

Effect::Effect()
{
#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_inputEventFilter.get());
#else
    KWin::input()->prependInputEventFilter(m_inputEventFilter.get());
#endif

    reconfigure(ReconfigureAll);
}

Effect::~Effect()
{
    KWin::input()->uninstallInputEventFilter(m_inputEventFilter.get());
}

void Effect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)

    Config::instance().read(m_inputEventFilter, m_input, m_windowInfoProvider);
}