#pragma once

#include "mockwindowdataprovider.h"
#include <QTest>

class TestCondition : public QObject
{
    Q_OBJECT
public:
    TestCondition();
private slots:
    void isSatisfied_noActiveWindow_returnsFalse();
    void isSatisfied_negatedAndNoActiveWindow_returnsFalse();
    void isSatisfied_noSubConditions_returnsTrue();
    void isSatisfied_negatedAndNoSubConditions_returnsFalse();
    void isSatisfied_windowClassRegexMatchesResourceClass_returnsTrue();
    void isSatisfied_windowClassRegexMatchesResourceName_returnsTrue();
    void isSatisfied_windowClassRegexDoesntMatch_returnsFalse();
    void isSatisfied_negatedAndWindowClassRegexMatches_returnsFalse();
    void isSatisfied_negatedAndWindowClassRegexDoesntMatch_returnsTrue();
    void isSatisfied_windowStateDoesntMatch_returnsFalse();
    void isSatisfied_windowStateMatches_returnsTrue();
    void isSatisfied_windowStateMatchesEither_returnsTrue();
    void isSatisfied_negatedAndWindowStateDoesntMatch_returnsTrue();
    void isSatisfied_negatedAndWindowStateMatches_returnsFalse();
    void isSatisfied_negatedAndWindowStateMatchesEither_returnsFalse();
    void isSatisfied_windowClassMatchesAndStateDoesnt_returnsFalse();
    void isSatisfied_windowClassDoesntMatchAndStateDoes_returnsFalse();
    void isSatisfied_windowClassMatchesAndStateMatches_returnsTrue();
    void isSatisfied_negatedAndWindowClassMatchesAndStateDoesnt_returnsTrue();
    void isSatisfied_negatedAndWindowClassDoesntMatchAndStateDoes_returnsTrue();
    void isSatisfied_negatedAndWindowClassMatchesAndStateMatches_returnsFalse();
private:
    const std::shared_ptr<MockWindowDataProvider> m_noActiveWindow;
    const std::shared_ptr<MockWindowDataProvider> m_normalWindow;
    const std::shared_ptr<MockWindowDataProvider> m_maximizedWindow;
    const std::shared_ptr<MockWindowDataProvider> m_fullscreenWindow;
};

