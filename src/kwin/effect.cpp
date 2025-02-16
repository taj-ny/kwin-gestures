#include "effect.h"

#include "core/pixelgrid.h"
#include "core/rendertarget.h"
#include "core/renderviewport.h"
#include "effect/effecthandler.h"
#include "opengl/glshader.h"
#include "opengl/glshadermanager.h"
#include "opengl/glvertexbuffer.h"
#include "window.h"

#include "libgestures/actions/callback.h"
#include "libgestures/animations/animation.h"
#include "libgestures/conditions/callback.h"
#include "libgestures/yaml_convert.h"

#include <QDir>
#include <QLoggingCategory>
#include <QPalette>

Q_LOGGING_CATEGORY(KWIN_GESTURES, "kwin_gestures", QtWarningMsg)

const QString configFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kwingestures.yml";

Effect::Effect()
{
    libgestures::AnimationHandler::setImplementation(m_animationHandler);
    libgestures::Input::setImplementation(new KWinInput);
    libgestures::WindowInfoProvider::setImplementation(new KWinWindowInfoProvider);
    registerBuiltinGestures();


#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_inputEventFilter.get());
#else
    KWin::input()->prependInputEventFilter(m_inputEventFilter.get());
#endif

    m_animationHandler->setAnimationDuration(animationTime(std::chrono::milliseconds(250)));

    reconfigure(ReconfigureAll);

    if (!QFile::exists(configFile)) {
        QFile(configFile).open(QIODevice::WriteOnly);
        configureWatcher();
    }

    connect(&m_configFileWatcher, &QFileSystemWatcher::directoryChanged, this, &Effect::slotConfigDirectoryChanged);
    connect(&m_configFileWatcher, &QFileSystemWatcher::fileChanged, this, &Effect::slotConfigFileChanged);
}

Effect::~Effect()
{
    if (KWin::input()) {
        KWin::input()->uninstallInputEventFilter(m_inputEventFilter.get());
    }
}

void Effect::slotConfigFileChanged()
{
    if (!m_configFileWatcher.files().contains(configFile)) {
        m_configFileWatcher.addPath(configFile);
    }

    reconfigure(ReconfigureAll);
}

void Effect::slotConfigDirectoryChanged()
{
    if (!m_configFileWatcher.files().contains(configFile) && QFile::exists(configFile)) {
        m_configFileWatcher.addPath(configFile);
        reconfigure(ReconfigureAll);
    }
}

void Effect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)

    try {
        const auto config = YAML::LoadFile(configFile.toStdString());

        m_autoReload = config["autoreload"].as<bool>(true);

        const auto animationsNode = config["animations"];
        if (animationsNode.IsDefined()) {
            m_animationHandler->setCurve(animationsNode["curve"].as<QEasingCurve>(QEasingCurve::InOutQuad));
            m_animationHandler->setProgressCurve(animationsNode["curve_progress"].as<QEasingCurve>(QEasingCurve::Linear));
            m_overlayColor = animationsNode["overlay_color"].as<QColor>(QColor());
            m_overlayOpacity = animationsNode["overlay_opacity"].as<qreal>(m_overlayOpacity);
        }

        auto gestureRecognizer = config["touchpad"].as<std::shared_ptr<libgestures::GestureRecognizer>>();
        m_inputEventFilter->setTouchpadGestureRecognizer(gestureRecognizer);
    } catch (const YAML::Exception &e) {
        qCritical(KWIN_GESTURES).noquote() << QStringLiteral("Failed to load configuration: ") + QString::fromStdString(e.msg)
                + " (line " + QString::number(e.mark.line) + ", column " + QString::number(e.mark.column) + ")";
        return;
    }

    if (m_autoReload) {
        configureWatcher();
    } else {
        m_configFileWatcher.removePaths(m_configFileWatcher.files() + m_configFileWatcher.directories());
    }
}

void Effect::configureWatcher()
{
    m_configFileWatcher.addPath(configFile);
    m_configFileWatcher.addPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
}

// This method sucks
void Effect::registerBuiltinGestures()
{
    std::function<bool()> hasActiveWindowCondition = [this]() {
        return activeWindow();
    };
    std::function<bool()> isMovableAndResizeableCondition = [this]() {
        return activeWindow()->isMovable() && activeWindow()->window()->isResizable();
    };

    // It would be better to toggle dragging directly instead of sending input, but I'm too lazy to find the method
    // for that.
    auto builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->setCompatibleGestureTypes(libgestures::GestureType::Swipe);
    builtin->addAction(libgestures::On::Begin, [](auto &) {
        libgestures::Input::implementation()->mouseButton(BTN_LEFT, true);
    });
    builtin->addActions([](auto &actions) {
        std::vector<libgestures::InputAction> inputActions{libgestures::InputAction()};
        inputActions[0].mouseMoveRelativeByDelta = true;
        auto action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::Update);
        actions.push_back(std::move(action));
    });
    builtin->addAction(libgestures::On::EndOrCancel, [](auto &) {
        libgestures::Input::implementation()->mouseButton(BTN_LEFT, false);
    });
    libgestures::BuiltinGesture::registerGesture("drag", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->setCompatibleGestureTypes(libgestures::GestureType::Swipe);
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->isMovable();
    });
    builtin->addAction(libgestures::On::Begin, [](auto &) {
        libgestures::Input::implementation()->keyboardKey(KEY_LEFTMETA, true);
        libgestures::Input::implementation()->mouseButton(BTN_LEFT, true);
    });
    builtin->addActions([](auto &actions) {
        std::vector<libgestures::InputAction> inputActions{libgestures::InputAction()};
        inputActions[0].mouseMoveRelativeByDelta = true;
        auto action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::Update);
        actions.push_back(std::move(action));
    });
    builtin->addAction(libgestures::On::EndOrCancel, [](auto &) {
        libgestures::Input::implementation()->keyboardKey(KEY_LEFTMETA, false);
        libgestures::Input::implementation()->mouseButton(BTN_LEFT, false);
    });
    libgestures::BuiltinGesture::registerGesture("window_drag", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->isCloseable();
    });
    builtin->setAction([this]() {
        activeWindow()->closeWindow();
    });
    builtin->addGeometryAnimation([this]() {
        const auto geometry = activeWindow()->frameGeometry();
        return QRectF(geometry.x() + geometry.width() / 2, geometry.y() + geometry.height() / 2, 0, 0);
    }, false, true, true);
    libgestures::BuiltinGesture::registerGesture("window_close", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return !activeWindow()->isFullScreen();
    });
    builtin->setAction([this]() {
        activeWindow()->window()->setFullScreen(true);
    });
    builtin->addGeometryAnimation([this]() {
        return clientArea(activeWindow(), KWin::FullScreenArea);
    }, true);
    libgestures::BuiltinGesture::registerGesture("window_fullscreen", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return !isMaximized(activeWindow());
    });
    builtin->setAction([this]() {
        activeWindow()->window()->maximize(KWin::MaximizeFull);
    });
    builtin->addGeometryAnimation([this] {
        return clientArea(activeWindow(), KWin::MaximizeArea);
    }, true);
    libgestures::BuiltinGesture::registerGesture("window_maximize", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->isMinimizable();
    });
    builtin->setAction([this]() {
        activeWindow()->minimize();
    });
    builtin->addGeometryAnimation([this]() {
        const auto w = activeWindow();
        const auto geometry = w->frameGeometry();
        return w->iconGeometry().isValid()
               ? w->iconGeometry()
               : QRectF(geometry.x() + geometry.width() / 2, geometry.y() + geometry.height() / 2, 0, 0);
    }, false, true, true);
    libgestures::BuiltinGesture::registerGesture("window_minimize", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return geometryRestore(activeWindow()).isValid()
            && (activeWindow()->isFullScreen() || isMaximized(activeWindow()) || activeWindow()->window()->quickTileMode() != KWin::QuickTileFlag::None);
    });
    builtin->setAction([this]() {
        const auto w = activeWindow();
        if (w->isFullScreen()) {
            w->window()->setFullScreen(false);
        } else if (w->window()->maximizeMode() != KWin::MaximizeRestore) {
            w->window()->maximize(KWin::MaximizeRestore);
        } else {
            w->window()->setQuickTileModeAtCurrentPosition(KWin::QuickTileFlag::None);
        }
    });
    builtin->addGeometryAnimation([this]() {
        return geometryRestore(activeWindow());
    }, true);
    libgestures::BuiltinGesture::registerGesture("window_restore", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->quickTileMode() != KWin::QuickTileFlag::Left;
    });
    builtin->setAction([this]() {
        activeWindow()->window()->setQuickTileModeAtCurrentPosition(KWin::QuickTileFlag::Left);
    });
    builtin->addGeometryAnimation([this]() {
        auto rect = clientArea(activeWindow(), KWin::PlacementArea);
        rect.setWidth(rect.width() / 2);
        return rect;
    }, true);
    libgestures::BuiltinGesture::registerGesture("window_tile_left", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->quickTileMode() != KWin::QuickTileFlag::Right;
    });
    builtin->setAction([this]() {
        activeWindow()->window()->setQuickTileModeAtCurrentPosition(KWin::QuickTileFlag::Right);
    });
    builtin->addGeometryAnimation([this]() {
        auto rect = clientArea(activeWindow(), KWin::PlacementArea);
        rect.setX(rect.x() + rect.width() / 2);
        return rect;
    }, true);
    libgestures::BuiltinGesture::registerGesture("window_tile_right", std::move(builtin));
}


QRectF Effect::clientArea(const KWin::EffectWindow *w, const KWin::clientAreaOption &area) const
{
    return KWin::effects->clientArea(area, w->window()->output(), KWin::effects->currentDesktop());
}

QRectF Effect::geometryRestore(const KWin::EffectWindow *w) const
{
    if (w->isFullScreen()) {
        return w->window()->fullscreenGeometryRestore();
    }

    return w->window()->geometryRestore();
}

bool Effect::isMaximized(const KWin::EffectWindow *w) const
{
    return w->frameGeometry() == clientArea(w, KWin::MaximizeArea);
}

void Effect::prePaintScreen(KWin::ScreenPrePaintData &data, std::chrono::milliseconds presentTime)
{
    m_screenPresentTime = presentTime;
    m_previousRect = m_currentRect;
    if (m_animationHandler->type() != libgestures::GestureAnimation::None && m_animationHandler->geometryAnimation().isActive(presentTime)) {
        m_currentRect = m_animationHandler->geometryAnimation().value(presentTime).toRect();
        QRegion repaint(*m_currentRect);
        if (m_previousRect) {
            repaint += *m_previousRect;
        }

        data.paint += repaint;
        KWin::effects->addRepaint(repaint);
    } else {
        m_currentRect = {};
    }

    KWin::effects->prePaintScreen(data, presentTime);
}

void Effect::paintScreen(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, int mask, const QRegion &region, KWin::Output *screen)
{
    KWin::effects->paintScreen(renderTarget, viewport, mask, region, screen);
    if (m_animationHandler->type() != libgestures::GestureAnimation::Overlay || !m_currentRect) {
        return;
    }

    const auto scale = viewport.scale();
    const auto screenSize = (screen->pixelSize() * scale);

    QRegion effectiveRegion;
    for (const auto &dirtyRect : (region & *m_currentRect)) {
        effectiveRegion += KWin::snapToPixelGrid(KWin::scaledRect(dirtyRect, scale));
    }

    auto *vbo = KWin::GLVertexBuffer::streamingBuffer();
    vbo->reset();
    vbo->setAttribLayout(std::span(KWin::GLVertexBuffer::GLVertex2DLayout), sizeof(KWin::GLVertex2D));


    const int vertexCount = effectiveRegion.rectCount() * 6;
    if (auto result = vbo->map<KWin::GLVertex2D>(vertexCount)) {
        auto map = *result;

        size_t vboIndex = 0;

        for (const QRect &rect : effectiveRegion) {
            const float x0 = rect.left();
            const float y0 = rect.top();
            const float x1 = rect.right();
            const float y1 = rect.bottom();

            const float u0 = x0 / screenSize.width();
            const float v0 = 1.0f - y0 / screenSize.height();
            const float u1 = x1 / screenSize.width();
            const float v1 = 1.0f - y1 / screenSize.height();

            // first triangle
            map[vboIndex++] = KWin::GLVertex2D{
                .position = QVector2D(x0, y0),
                .texcoord = QVector2D(u0, v0),
            };
            map[vboIndex++] = KWin::GLVertex2D{
                .position = QVector2D(x1, y1),
                .texcoord = QVector2D(u1, v1),
            };
            map[vboIndex++] = KWin::GLVertex2D{
                .position = QVector2D(x0, y1),
                .texcoord = QVector2D(u0, v1),
            };

            // second triangle
            map[vboIndex++] = KWin::GLVertex2D{
                .position = QVector2D(x0, y0),
                .texcoord = QVector2D(u0, v0),
            };
            map[vboIndex++] = KWin::GLVertex2D{
                .position = QVector2D(x1, y0),
                .texcoord = QVector2D(u1, v0),
            };
            map[vboIndex++] = KWin::GLVertex2D{
                .position = QVector2D(x1, y1),
                .texcoord = QVector2D(u1, v1),
            };
        }

        vbo->unmap();
    } else {
        qWarning() << "Failed to map vertex buffer";
        return;
    }

    vbo->bindArrays();

    KWin::ShaderBinder binder(KWin::ShaderTrait::UniformColor | KWin::ShaderTrait::TransformColorspace);
    binder.shader()->setUniform(KWin::GLShader::Mat4Uniform::ModelViewProjectionMatrix, viewport.projectionMatrix());
    binder.shader()->setColorspaceUniforms(KWin::ColorDescription::sRGB, renderTarget.colorDescription(), KWin::RenderingIntent::Perceptual);
    QColor color = m_overlayColor.isValid() ? m_overlayColor : QApplication::palette().color(QPalette::Highlight);
    color.setAlphaF(1.0);
    binder.shader()->setUniform(KWin::GLShader::ColorUniform::Color, color);

    const auto opacity = m_animationHandler->geometryOpacityAnimation().value(m_screenPresentTime) * m_overlayOpacity;
    if (opacity < 1.0) {
        glEnable(GL_BLEND);
        glBlendColor(0, 0, 0, opacity);
        glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    }

    vbo->draw(GL_TRIANGLES, 0, vertexCount);

    if (opacity < 1.0) {
        glDisable(GL_BLEND);
    }
    vbo->unbindArrays();
}

KWin::EffectWindow *Effect::activeWindow()
{
    return KWin::effects->activeWindow();
}

void Effect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds presentTime)
{
    if (m_animationHandler->window() == w) {
        if (m_currentRect) {
            data.paint += *m_currentRect;
        }
        if (m_previousRect) {
            data.paint += *m_previousRect;
        }

        data.setTransformed();
        m_animationHandler->repaint();
    }

    KWin::effects->prePaintWindow(w, data, presentTime);
}

void Effect::paintWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, KWin::EffectWindow *w, int mask, QRegion region, KWin::WindowPaintData &data) {
    if (m_animationHandler->suppressAnimations(w)) {
        data.setXTranslation(0);
        data.setYTranslation(0);
        data.setZTranslation(0);
        data.setScale(QVector3D(1, 1, 1));
        data.setCrossFadeProgress(1);
        data.setOpacity(1);
    }

    if (m_animationHandler->type() == libgestures::GestureAnimation::Window
        && m_animationHandler->window() == w) {
        if (m_animationHandler->geometryAnimation().isActive(m_screenPresentTime)) {
            m_animationQuality = m_overlayOpacity;

            // TODO The window jumps around by 1 px due to rounding
            const auto from = w->frameGeometry();
            const auto current = m_animationHandler->geometryAnimation().value(m_screenPresentTime);
            if (m_animationHandler->resizesWindow()
                && (
                   (m_animationHandler->geometryAnimation().isCompleted(m_screenPresentTime) && from.size() != current.size())
                    || (std::abs(from.width() - current.width()) >= m_animationQuality || std::abs(from.height() - current.height()) >= m_animationQuality)
                )) {
                w->window()->moveResize(current);
            }

            data.setXTranslation(current.x() - from.x());
            data.setYTranslation(current.y() - from.y());
            data.setXScale(current.width() / (qreal)from.width());
            data.setYScale(current.height() / (qreal)from.height());

        }

        if (m_animationHandler->geometryOpacityAnimation().isActive(m_screenPresentTime)) {
            data.setOpacity(m_animationHandler->geometryOpacityAnimation().value(m_screenPresentTime));
        }
    }

    KWin::effects->paintWindow(renderTarget, viewport, w, mask, region, data);
}
