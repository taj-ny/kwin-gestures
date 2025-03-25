#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES)
Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_GESTURE)
Q_DECLARE_LOGGING_CATEGORY(LIBGESTURES_GESTURE_HANDLER)

QString since(const std::chrono::time_point<std::chrono::steady_clock> &start);
QString since(const std::chrono::time_point<std::chrono::steady_clock> &start, const std::chrono::time_point<std::chrono::steady_clock> &end);