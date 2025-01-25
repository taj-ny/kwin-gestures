#pragma once

#include "libgestures/actions/action.h"
#include "../mockwindowinfoprovider.h"
#include <QTest>

namespace libgestures
{

class TestAction : public QObject
{
Q_OBJECT
private slots:
    void init();

    void satisfiesConditions_data();
    void satisfiesConditions();

    void canExecute_nonRepeatingNotExecuted_returnsTrue();
    void canExecute_repeatingNotExecuted_returnsTrue();
    void canExecute_nonRepeatingExecutedOnce_returnsFalse();
    void canExecute_repeatingExecutedOnce_returnsTrue();
    void canExecute_nonRepeatingExecutedOnceAndEnded_returnsTrue();
    void canExecute_nonRepeatingExecutedOnceAndCancelled_returnsTrue();

    void onGestureUpdated_notRepeating_doesntExecuteAction();
    void onGestureUpdated_repeating_data();
    void onGestureUpdated_repeating();
private:
    const std::shared_ptr<Condition> m_satisfiedCondition = std::make_shared<Condition>(std::make_shared<MockWindowInfoProvider>(WindowInfo("Firefox", "firefox", "firefox", WindowState::Unimportant)));
    const std::shared_ptr<Condition> m_unsatisfiedCondition = std::make_shared<Condition>(std::make_shared<MockWindowInfoProvider>(std::nullopt));

    std::shared_ptr<GestureAction> m_action;
};

}