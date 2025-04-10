#pragma once

#include <libinputactions/actions/action.h>

#include <gmock/gmock.h>

namespace libinputactions
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