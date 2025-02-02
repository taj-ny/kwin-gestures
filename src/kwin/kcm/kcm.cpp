#include "kcm.h"

#include <KPluginFactory>

#include <QFile>

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
}

}

#include "kcm.moc"
#include "moc_kcm.cpp"
