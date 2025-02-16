#pragma once

#include "libgestures/animations/handler.h"

#include "effect/effectwindow.h"

#include <unordered_set>

class KWinAnimationHandler : public libgestures::AnimationHandler
{
public:
    KWinAnimationHandler();

    void startGeometry(const libgestures::GestureAnimation &type, const QRectF &to, const bool &resizeDuring = false, const bool &keepLastFrame = false, const bool &fadeOut = false) override;

    void repaint() override;

    KWin::EffectWindow *window();
    bool suppressAnimations(const KWin::EffectWindow *w) const;

private:
    void endedOrCancelled() override;
    QRectF windowGeometry() const override;

    KWin::EffectWindow *m_currentWindow;
    QRectF m_geometryRestore;

    std::unordered_set<const KWin::EffectWindow *> m_suppressAnimations;
};