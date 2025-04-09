#pragma once

#include <libinputactions/gestures/gesture.h>
#include <QTest>

namespace libinputactions
{

class TestGesture : public QObject
{
Q_OBJECT
private slots:
    void thresholdReached_data();
    void thresholdReached();
};

}