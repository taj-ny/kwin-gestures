#include "gestureseffect.h"

namespace KWin
{

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(GesturesEffect,
                                      "metadata.json",
                                      return GesturesEffect::supported();
                                      ,
                                      return GesturesEffect::enabledByDefault();)

} // namespace KWin

#include "main.moc"
