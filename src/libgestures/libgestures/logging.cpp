#include "logging.h"

Q_LOGGING_CATEGORY(LIBGESTURES, "libgestures", QtWarningMsg)
Q_LOGGING_CATEGORY(LIBGESTURES_GESTURE, "libgestures.gesture", QtWarningMsg)
Q_LOGGING_CATEGORY(LIBGESTURES_GESTURE_HANDLER, "libgestures.gesture.handler", QtWarningMsg)

QString since(const std::chrono::time_point<std::chrono::steady_clock> &start)
{
    return qPrintable(QString::number(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count()) + " µs");
}

QString since(const std::chrono::time_point<std::chrono::steady_clock> &start, const std::chrono::time_point<std::chrono::steady_clock> &end)
{
    return qPrintable(QString::number(std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) + " µs");
}