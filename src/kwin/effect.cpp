#include "effect.h"

#include "core/pixelgrid.h"
#include "core/renderviewport.h"
#include "effect/effecthandler.h"
#include "opengl/glshader.h"
#include "opengl/glshadermanager.h"
#include "opengl/glvertexbuffer.h"
#include "window.h"

#include "libgestures/actions/callback.h"
#include "libgestures/conditions/callback.h"
#include "libgestures/yaml_convert.h"

#include <QDir>
#include <QLoggingCategory>
#include <QPalette>

Q_LOGGING_CATEGORY(KWIN_GESTURES, "kwin_gestures", QtWarningMsg)

const QString configFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kwingestures.yml";

Effect::Effect()
{
    libgestures::Input::setImplementation(new KWinInput);
    libgestures::WindowInfoProvider::setImplementation(new KWinWindowInfoProvider);
    registerBuiltinGestures();

#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_inputEventFilter.get());
#else
    KWin::input()->prependInputEventFilter(m_inputEventFilter.get());
#endif

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
            m_animationDuration = animationsNode["duration"].as<std::chrono::milliseconds>(m_animationDuration);
            m_curve = animationsNode["curve"].as<QEasingCurve>(m_curve);
            m_curveProgress = animationsNode["curve_progress"].as<QEasingCurve>(m_curveProgress);
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
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->setOneToOneActionFactory([](auto &actions) {
        std::vector<libgestures::InputAction> inputActions{libgestures::InputAction()};
        auto &input = inputActions[0];

        input.mousePress.push_back(BTN_LEFT);
        auto action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::Begin);
        actions.push_back(std::move(action));

        input = {};
        input.mouseMoveRelativeByDelta = true;
        action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::Update);
        actions.push_back(std::move(action));

        input = {};
        input.mouseRelease.push_back(BTN_LEFT);
        action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::EndOrCancel);
        actions.push_back(std::move(action));
    });
    libgestures::BuiltinGesture::registerGesture("drag", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->setCompatibleGestureTypes(libgestures::GestureType::Swipe);
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->isMovable();
    });
    builtin->setOneToOneActionFactory([](auto &actions) {
        std::vector<libgestures::InputAction> inputActions{libgestures::InputAction()};
        auto &input = inputActions[0];

        input.keyboardPress.push_back(KEY_LEFTMETA);
        input.mousePress.push_back(BTN_LEFT);
        auto action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::Begin);
        actions.push_back(std::move(action));

        input = {};
        input.mouseMoveRelativeByDelta = true;
        action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::Update);
        actions.push_back(std::move(action));

        input = {};
        input.keyboardRelease.push_back(KEY_LEFTMETA);
        input.mouseRelease.push_back(BTN_LEFT);
        action = std::make_unique<libgestures::InputGestureAction>(inputActions);
        action->setOn(libgestures::On::EndOrCancel);
        actions.push_back(std::move(action));
    });
    libgestures::BuiltinGesture::registerGesture("window_drag", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->isCloseable();
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            activeWindow()->closeWindow();
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [](auto *w, auto &from, auto &to) {
            to = QRectF(from.x() + from.width() / 2, from.y() + from.height() / 2, 0, 0);
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_close", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return !activeWindow()->isFullScreen();
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            activeWindow()->window()->setFullScreen(true);
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [this](auto *w, auto &from, auto &to) {
            to = clientArea(KWin::FullScreenArea);
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_fullscreen", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return !isMaximized(activeWindow());
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            activeWindow()->window()->maximize(KWin::MaximizeFull);
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [](auto *w, auto &from, auto &to) {
            to = KWin::effects->clientArea(KWin::MaximizeArea, KWin::effects->activeScreen(), KWin::effects->currentDesktop());
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_maximize", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->isMinimizable();
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            activeWindow()->minimize();
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [](auto *w, auto &from, auto &to) {
            to = w->iconGeometry().isValid()
                ? w->iconGeometry()
                : QRectF(from.x() + from.width() / 2, from.y() + from.height() / 2, 0, 0);
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_minimize", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition([this]() {
        return geometryRestore(activeWindow()).isValid()
            && (activeWindow()->isFullScreen() || isMaximized(activeWindow()) || activeWindow()->window()->quickTileMode() != KWin::QuickTileFlag::None);
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            const auto w = activeWindow();
            if (w->isFullScreen()) {
                w->window()->setFullScreen(false);
            } else if (isMaximized(w)) {
                w->window()->maximize(KWin::MaximizeRestore);
            } else {
                w->window()->setQuickTileModeAtCurrentPosition(KWin::QuickTileFlag::None);
            }
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [this](auto *w, auto &from, auto &to) {
            to = geometryRestore(w);
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_restore", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->quickTileMode() != KWin::QuickTileFlag::Left;
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            activeWindow()->window()->setQuickTileModeAtCurrentPosition(KWin::QuickTileFlag::Left);
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [this](auto *w, auto &from, auto &to) {
            auto rect = clientArea(KWin::PlacementArea);
            rect.setWidth(rect.width() / 2);
            to = rect;
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_tile_left", std::move(builtin));

    builtin = std::make_unique<libgestures::BuiltinGesture>();
    builtin->addRequiredCondition(hasActiveWindowCondition);
    builtin->addRequiredCondition(isMovableAndResizeableCondition);
    builtin->addRequiredCondition([this]() {
        return activeWindow()->window()->quickTileMode() != KWin::QuickTileFlag::Right;
    });
    builtin->setSingleActionFactory([this]() {
        return std::make_unique<libgestures::CallbackGestureAction>([this](const qreal &) {
            activeWindow()->window()->setQuickTileModeAtCurrentPosition(KWin::QuickTileFlag::Right);
        });
    });
    builtin->setAnimationFactory(libgestures::GestureAnimation::Overlay, [this](auto &actions) {
        createRectangleOverlayAnimation(actions, [this](auto *w, auto &from, auto &to) {
            auto rect = clientArea(KWin::PlacementArea);
            rect.setX(rect.x() + rect.width() / 2);
            to = rect;
        });
    });
    libgestures::BuiltinGesture::registerGesture("window_tile_right", std::move(builtin));
}

void Effect::createRectangleOverlayAnimation(
    std::vector<std::unique_ptr<libgestures::GestureAction>> &actions,
    const std::function<void(KWin::EffectWindow *w, QRectF &from, QRectF &to)> &animation
)
{
    auto action = std::make_unique<libgestures::CallbackGestureAction>([this, animation](const auto &) {
        const auto w = activeWindow();
        m_from = w->frameGeometry();
        animation(activeWindow(), m_from, m_to);
        set(m_from, 0.0);
        animateOpacity(0.0, 1.0);
    });
    action->setOn(libgestures::On::Begin);
    actions.push_back(std::move(action));

    action = std::make_unique<libgestures::CallbackGestureAction>([this](const auto &progress) {
        set(interpolate(m_from, m_to, progress), m_currentOpacity);
    });
    action->setOn(libgestures::On::Update);
    actions.push_back(std::move(action));

    action = std::make_unique<libgestures::CallbackGestureAction>([this](const auto &progress) {
        if (progress >= 0.8) {
            // If a gesture is performed very quickly, the rectangle may not each the end of the animation.
            set(m_to, m_currentOpacity);
        }
        animateOpacity(1.0, 0.0);
    });
    action->setOn(libgestures::On::End);
    actions.push_back(std::move(action));

    action = std::make_unique<libgestures::CallbackGestureAction>([this](const auto &) {
        animateOpacity(1.0, 0.0);
    });
    action->setOn(libgestures::On::Cancel);
    actions.push_back(std::move(action));
}

QRectF Effect::interpolate(const QRectF &from, const QRectF &to, qreal progress)
{
    progress = m_curveProgress.valueForProgress(progress);
    qreal x = from.x() + (to.x() - from.x()) * progress;
    qreal y = from.y() + (to.y() - from.y()) * progress;
    qreal width = from.width() + (to.width() - from.width()) * progress;
    qreal height = from.height() + (to.height() - from.height()) * progress;

    return {x, y, width, height};
}

qreal Effect::interpolate(const qreal &from, const qreal &to, qreal progress)
{
    progress = m_curve.valueForProgress(progress);
    return from + (to - from) * progress;
}

void Effect::animateOpacity(const qreal &from, const qreal &to)
{
    m_animationStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

    m_fromOpacity = from;
    m_toOpacity = to;

    m_hasAnimation = true;
    m_needsRepaints = true;
    KWin::effects->addRepaintFull();
}

void Effect::set(const QRectF &rect, const qreal &opacity)
{
    m_currentRect = rect;
    m_currentOpacity = opacity;
    KWin::effects->addRepaintFull();
}

QRectF Effect::clientArea(const KWin::clientAreaOption &area) const
{
    return KWin::effects->clientArea(area, KWin::effects->activeScreen(), KWin::effects->currentDesktop());
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
    return w->frameGeometry() == clientArea(KWin::MaximizeArea);
}

void Effect::prePaintScreen(KWin::ScreenPrePaintData &data, std::chrono::milliseconds presentTime)
{
    if (m_hasAnimation) {
        const qreal progress = std::clamp((presentTime.count() - m_animationStart.count()) / (qreal)m_animationDuration.count(), 0.0, 1.0);
        m_currentOpacity = interpolate(m_fromOpacity, m_toOpacity, progress);

        if (qFuzzyCompare(progress, 1.0)) {
            m_hasAnimation = false;
        }
    }

    KWin::effects->prePaintScreen(data, presentTime);
}

void Effect::paintScreen(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, int mask, const QRegion &region, KWin::Output *screen)
{
    KWin::effects->paintScreen(renderTarget, viewport, mask, region, screen);
    if (qFuzzyCompare(m_currentOpacity, 0.0)) {
        return;
    }

    const auto scale = viewport.scale();
    const auto screenSize = (screen->pixelSize() * scale);

    QRegion effectiveRegion;
    for (const auto &dirtyRect : (region & m_currentRect.toRect())) {
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

    KWin::ShaderBinder binder(KWin::ShaderTrait::UniformColor);
    binder.shader()->setUniform(KWin::GLShader::Mat4Uniform::ModelViewProjectionMatrix, viewport.projectionMatrix());
    QColor color = m_overlayColor.isValid() ? m_overlayColor : QApplication::palette().color(QPalette::Highlight);
    color.setAlphaF(m_currentOpacity * m_overlayOpacity);
    binder.shader()->setUniform(KWin::GLShader::ColorUniform::Color, color);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    vbo->draw(GL_TRIANGLES, 0, vertexCount);
    glDisable(GL_BLEND);

    vbo->unbindArrays();
}

void Effect::postPaintScreen()
{
    if (m_hasAnimation || m_needsRepaints) {
        KWin::effects->addRepaintFull();
    }

    if (!m_hasAnimation) {
        m_needsRepaints = false;
    }

    KWin::effects->postPaintScreen();
}

KWin::EffectWindow *Effect::activeWindow()
{
    return KWin::effects->activeWindow();
}