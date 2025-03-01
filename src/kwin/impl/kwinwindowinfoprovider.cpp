#include "kwinwindowinfoprovider.h"
#include "effect/effecthandler.h"
#include "window.h"

std::optional<const libgestures::WindowInfo> KWinWindowInfoProvider::activeWindow() const
{
    const auto window = KWin::effects->activeWindow();
    if (!window)
        return std::nullopt;

    libgestures::WindowStates state = static_cast<libgestures::WindowState>(0);
    if (window->isFullScreen())
        state = state | libgestures::WindowState::Fullscreen;
    if (window->window()->maximizeMode() == KWin::MaximizeMode::MaximizeFull)
        state = state | libgestures::WindowState::Maximized;

    return libgestures::WindowInfo(window->caption(), window->window()->resourceClass(), window->window()->resourceName(), state);
}