#include "effect/effecthandler.h"
#include "kwinwindowdataprovider.h"
#include "window.h"

std::optional<const WindowData> KWinWindowDataProvider::getDataForActiveWindow() const
{
    const auto window = KWin::effects->activeWindow();
    if (!window)
        return std::nullopt;

    WindowState state = WindowState::Unimportant;
    if (window->isFullScreen())
        state = state | WindowState::Fullscreen;
    if (window->frameGeometry() == KWin::effects->clientArea(KWin::MaximizeArea, KWin::effects->activeScreen(), KWin::effects->currentDesktop()))
        state = state | WindowState::Maximized;

    return WindowData(window->caption(), window->window()->resourceClass(), window->window()->resourceName(), state);
}