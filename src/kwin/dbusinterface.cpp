#include "dbusinterface.h"

#include "libgestures/gestures/stroke.h"

#include "effect/effecthandler.h"

static QString s_service = "dev.taj-ny.kwin-gestures";
static QString s_path = "/KWinGestures";

DBusInterface::DBusInterface(GestureInputEventFilter *filter)
    : m_filter(filter)
{
    m_bus.registerService(s_service);
    m_bus.registerObject(s_path, this, QDBusConnection::ExportAllSlots);
}

DBusInterface::~DBusInterface()
{
    m_bus.unregisterService(s_service);
    m_bus.unregisterObject(s_path);
}

void DBusInterface::recordStroke(const QDBusMessage &message)
{
    KWin::effects->showOnScreenMessage("KWin Gestures is recording input. Perform a stroke gesture by moving your mouse or performing a touchpad swipe. Recording will end after 250 ms of inactivity.");

    message.setDelayedReply(true);
    auto reply = message.createReply();

    connect(m_filter, &GestureInputEventFilter::strokeRecordingFinished, this, [this, reply](const auto &stroke) {
        QByteArray bytes;
        const auto &points = stroke.points();
        for (size_t i = 0; i < points.size(); i++) {
            // All values range from -1 to 1
            bytes.push_back(static_cast<char>(points[i].x * 100));
            bytes.push_back(static_cast<char>(points[i].y * 100));
            bytes.push_back(static_cast<char>(points[i].t * 100));
            bytes.push_back(static_cast<char>(points[i].alpha * 100));
        }

        static_cast<QDBusMessage>(reply) << QString("'%1'").arg(bytes.toBase64()); // wtf
        m_bus.send(reply);

        KWin::effects->hideOnScreenMessage();
    }, Qt::SingleShotConnection);
    m_filter->recordStroke();
}