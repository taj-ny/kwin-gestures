#pragma once

#include "libgestures/triggers/directionalmotiontrigger.h"

#include <QTest>

namespace libgestures
{

class TestDirectionalMotionTrigger : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void canUpdate_data();
    void canUpdate();

private:
    std::unique_ptr<DirectionalMotionTrigger> m_motionTrigger;
};

}