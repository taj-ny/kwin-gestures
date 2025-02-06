#include "effect.h"

#include "effect/effecthandler.h"
#include "window.h"

#include "libgestures/actions/callback.h"
#include "libgestures/yaml_convert.h"

#include <QDir>
#include <QLoggingCategory>

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

void Effect::registerBuiltinGestures()
{
    libgestures::BuiltinGesture::registerGesture("drag", std::unique_ptr<libgestures::BuiltinGesture>(&(new libgestures::BuiltinGesture)
        ->setCompatibleGestureTypes(libgestures::GestureType::Swipe)
        .setAssigner([](auto gesture, auto &) {
            libgestures::InputAction input;

            input.mousePress.push_back(BTN_LEFT);
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::InputGestureAction())
                ->setSequence({input})
                .setOn(libgestures::On::Begin)
            ));

            input = {};
            input.mouseMoveRelativeByDelta = true;
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::InputGestureAction)
                ->setSequence({input})
                .setOn(libgestures::On::Update)
            ));

            input = {};
            input.mouseRelease.push_back(BTN_LEFT);
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::InputGestureAction)
                ->setSequence({input})
                .setOn(libgestures::On::EndOrCancel)
            ));
        })
    ));

    libgestures::BuiltinGesture::registerGesture("window_drag", std::unique_ptr<libgestures::BuiltinGesture>(&(new libgestures::BuiltinGesture)
        ->setCompatibleGestureTypes(libgestures::GestureType::Swipe)
        .setAssigner([](auto gesture, auto &) {
            libgestures::InputAction input;

            input.keyboardPress.push_back(KEY_LEFTMETA);
            input.mousePress.push_back(BTN_LEFT);
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::InputGestureAction())
                ->setSequence({input})
                .setOn(libgestures::On::Begin)
            ));

            input = {};
            input.mouseMoveRelativeByDelta = true;
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::InputGestureAction)
                ->setSequence({input})
                .setOn(libgestures::On::Update)
            ));

            input = {};
            input.keyboardRelease.push_back(KEY_LEFTMETA);
            input.mouseRelease.push_back(BTN_LEFT);
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::InputGestureAction)
                ->setSequence({input})
                .setOn(libgestures::On::EndOrCancel)
            ));
        })
    ));

    registerSimpleShortcutBuiltinGesture("window_close", "kwin", "Window Close");
    registerSimpleShortcutBuiltinGesture("window_maximize", "kwin", "Window Maximize");
    registerSimpleShortcutBuiltinGesture("window_minimize", "kwin", "Window Minimize");
}

void Effect::registerSimpleShortcutBuiltinGesture(const QString &id, const QString &component, const QString &shortcut)
{
    libgestures::BuiltinGesture::registerGesture(id, std::unique_ptr<libgestures::BuiltinGesture>(&(new libgestures::BuiltinGesture)
        ->setAssigner([component, shortcut](auto gesture, auto &config) {
            gesture->addAction(std::unique_ptr<libgestures::GestureAction>(&(new libgestures::PlasmaGlobalShortcutGestureAction)
                ->setComponent(component)
                .setShortcut(shortcut)
                .setOn(config.isInstant ? libgestures::On::Begin : libgestures::On::End)
            ));
        })
    ));
}