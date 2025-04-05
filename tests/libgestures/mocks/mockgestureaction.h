#pragma once

#include "libgestures/actions/action.h"

#include <gmock/gmock.h>

namespace libgestures
{

class MockGestureAction : public GestureAction
{
public:
    MockGestureAction() = default;

    MOCK_METHOD(void, execute, (), (override));
    MOCK_METHOD(bool, canExecute, (), (const, override));
    MOCK_METHOD(const bool &, executed, (), (const, override));
    MOCK_METHOD(void, tryExecute, (), (override));

    MOCK_METHOD(void, gestureStarted, (), (override));
    MOCK_METHOD(void, gestureUpdated, (const qreal &, const QPointF &), (override));
    MOCK_METHOD(void, gestureEnded, (), (override));
    MOCK_METHOD(void, gestureCancelled, (), (override));
};

}