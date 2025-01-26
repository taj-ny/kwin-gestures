#include "effect/effecthandler.h"
#include "kwinwindowinfoprovider.h"
#include "window.h"

std::optional<const libgestures::WindowInfo> KWinWindowInfoProvider::activeWindow() const
{
    const auto window = KWin::effects->activeWindow();
    if (!window)
        return std::nullopt;

    libgestures::WindowStates state = static_cast<libgestures::WindowState>(0);
    if (window->isFullScreen())
        state = state | libgestures::WindowState::Fullscreen;
    if (window->frameGeometry() == KWin::effects->clientArea(KWin::MaximizeArea, KWin::effects->activeScreen(), KWin::effects->currentDesktop()))
        state = state | libgestures::WindowState::Maximized;

    return libgestures::WindowInfo(window->caption(), window->window()->resourceClass(), window->window()->resourceName(), state);
}