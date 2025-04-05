#pragma once

#include "mocks/mockgestureaction.h"

#include <QTest>

namespace libgestures
{

class TestActionInterval : public QObject
{
    Q_OBJECT

private slots:
    void matches_data();
    void matches();
};

}