#include "kcm.h"

#include <KPluginFactory>

#include <QClipboard>
#include <QtDBus/QDBusInterface>
#include <QFile>
#include <QObject>

namespace KWin
{

K_PLUGIN_CLASS(KWinGesturesKCM)

KWinGesturesKCM::KWinGesturesKCM(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());
    setButtons(Button::NoAdditionalButton);

    QFile about(":/effects/kwin_gestures/kcm/about.html");
    if (about.open(QIODevice::ReadOnly)) {
        const auto html = about.readAll()
                              .replace("${version}", ABOUT_VERSION_STRING)
                              .replace("${repo}", "https://github.com/taj-ny/kwin-gestures");
        ui.aboutText->setHtml(html);
    }

    connect(ui.copy, &QPushButton::pressed, this, &KWinGesturesKCM::slotCopyPoints);
    connect(ui.recordStroke, &QPushButton::pressed, this, &KWinGesturesKCM::slotRecordStroke);
}

void KWinGesturesKCM::slotCopyPoints()
{
    QApplication::clipboard()->setText(ui.strokePoints->text());
}

void KWinGesturesKCM::slotRecordStroke()
{
    QDBusInterface interface("dev.taj-ny.kwin-gestures", "/KWinGestures");
    auto call = interface.asyncCall("recordStroke");
    call.waitForFinished();
    ui.strokePoints->setText(call.reply().arguments().at(0).toString());

}

}

#include "kcm.moc"
#include "moc_kcm.cpp"
