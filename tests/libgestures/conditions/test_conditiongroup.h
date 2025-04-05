#pragma once

#include "libgestures/conditions/condition.h"

#include <QTest>

namespace libgestures
{

class TestConditionGroup : public QObject
{
    Q_OBJECT

private slots:
    void satisfies_data();
    void satisfies();
};

}