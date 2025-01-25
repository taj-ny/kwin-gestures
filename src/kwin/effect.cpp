#include "effect.h"
#include "libgestures/libgestures.h"
#include "libgestures/yaml_convert.h"

#include <QDir>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(KWIN_GESTURES, "kwin_gestures", QtWarningMsg)

const QString configFile = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/kwingestures.yml";

Effect::Effect()
{
    libgestures::libgestures::setInput(new KWinInput);
    libgestures::libgestures::setWindowInfoProvider(new KWinWindowInfoProvider);

#ifdef KWIN_6_2_OR_GREATER
    KWin::input()->installInputEventFilter(m_inputEventFilter.get());
#else
    KWin::input()->prependInputEventFilter(m_inputEventFilter.get());
#endif

    reconfigure(ReconfigureAll);
}

Effect::~Effect()
{
    if (KWin::input()) {
        KWin::input()->uninstallInputEventFilter(m_inputEventFilter.get());
    }
}

void Effect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)

    if (!QFile::exists(configFile)) {
        QFile(configFile).open(QIODevice::WriteOnly);
    }

    try {
        auto gestureRecognizer = YAML::LoadFile(configFile.toStdString())["touchpad"].as<std::shared_ptr<libgestures::GestureRecognizer>>();
        m_inputEventFilter->setTouchpadGestureRecognizer(gestureRecognizer);
    } catch (const YAML::Exception &e) {
        qCritical(KWIN_GESTURES).noquote() <<
            QStringLiteral("Failed to load configuration: ") + QString::fromStdString(e.msg)
            + " (line " + QString::number(e.mark.line) + ", column " + QString::number(e.mark.column) + ")";
    }
}