#pragma once

#include "libgestures/gestures/gesture.h"
#include <QTest>

namespace libgestures
{

class TestGesture : public QObject
{
Q_OBJECT
private slots:
    void thresholdReached_data();
    void thresholdReached();
};

} // namespace libgestures