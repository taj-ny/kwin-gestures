#include "kwinanimationhandler.h"

#include "core/output.h"
#include "effect/effecthandler.h"
#include "window.h"

KWinAnimationHandler::KWinAnimationHandler()
    : libgestures::AnimationHandler()
{
}

void KWinAnimationHandler::startGeometry(const libgestures::GestureAnimation &type, const QRectF &to, const bool &resizeDuring, const bool &keepLastFrame, const bool &fadeOut)
{
    m_currentWindow = KWin::effects->activeWindow();
    AnimationHandler::startGeometry(type, to, resizeDuring, keepLastFrame, fadeOut);

    if (this->resizesWindow()) {
        m_geometryRestore = m_currentWindow->window()->geometryRestore();
        if (!m_geometryRestore.isValid()) {
            m_geometryRestore = m_currentWindow->frameGeometry();
        }
    } else {
        m_geometryRestore = {};
    }
}

void KWinAnimationHandler::endedOrCancelled()
{
    if (const auto w = m_currentWindow) {
        const auto suppressAnimations = m_type == libgestures::GestureAnimation::Window;
        if (suppressAnimations) {
            m_suppressAnimations.insert(w);
        }
        if (m_keepLastFrame) {
            QTimer::singleShot(m_animationDuration, [this, w]() {
                m_currentWindow = nullptr;
                m_suppressAnimations.erase(w);
                m_geometryAnimation.stop();
                m_geometryOpacityAnimation.stop();
            });
        } else {
            QTimer::singleShot(m_animationDuration, [this, w]() {
                m_currentWindow = nullptr;
                m_suppressAnimations.erase(w);

                m_geometryAnimation.stop();
                m_geometryOpacityAnimation.stop();
            });
        }

        if (resizesWindow() && m_geometryRestore.isValid()) {
            w->window()->setGeometryRestore(m_geometryRestore);
        }
    }


    AnimationHandler::endedOrCancelled();
}

void KWinAnimationHandler::repaint()
{
    if (!m_currentWindow) {
        return;
    }

    switch (m_type) {
    case libgestures::GestureAnimation::Overlay:
        KWin::effects->addRepaintFull();
        break;

    case libgestures::GestureAnimation::Window:
        m_currentWindow->addLayerRepaint(m_currentWindow->window()->output()->geometry());
        break;
    }
}

bool KWinAnimationHandler::suppressAnimations(const KWin::EffectWindow *w) const
{
    return m_suppressAnimations.contains(w);
}

KWin::EffectWindow *KWinAnimationHandler::window()
{
    return m_currentWindow;
}

QRectF KWinAnimationHandler::windowGeometry() const
{
    return KWin::effects->activeWindow()->frameGeometry();
}