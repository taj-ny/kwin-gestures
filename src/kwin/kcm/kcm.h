#pragma once

#include "ui_kcm.h"

#include <KCModule>

namespace KWin
{

class KWinGesturesKCM : public KCModule
{
    Q_OBJECT

public:
    explicit KWinGesturesKCM(QObject *parent, const KPluginMetaData &data);

private slots:
    void slotCopyPoints();
    void slotRecordStroke();

private:
    ::Ui::KWinGesturesKCM ui;
};

}
