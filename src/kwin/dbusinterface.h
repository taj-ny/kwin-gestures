#include "inputfilter.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QObject>

class DBusInterface : public QObject
{
    Q_OBJECT
public:
    DBusInterface(GestureInputEventFilter *filter);
    ~DBusInterface();

public slots:
    Q_NOREPLY void recordStroke(const QDBusMessage &message);

private:
    GestureInputEventFilter *m_filter;
    QDBusConnection m_bus = QDBusConnection::sessionBus();
};