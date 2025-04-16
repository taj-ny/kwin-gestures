#pragma once

#include <libinputactions/conditions/condition.h>

#include <QTest>

namespace libinputactions
{

class TestConditionGroup : public QObject
{
    Q_OBJECT

private slots:
    void satisfies_data();
    void satisfies();
};

}