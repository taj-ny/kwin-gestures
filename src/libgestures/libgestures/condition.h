#pragma once

#include "windowinfoprovider.h"
#include <QRegularExpression>

namespace libgestures
{

class Condition
{
public:
    /**
     * @return @c false if no window is currently active. @c true if all specified subconditions are satisfied or if
     * none were specified.
     */
    bool isSatisfied() const;

    /**
     * @param windowClassRegex If empty, the subcondition will always be satisfied.
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    Condition &setWindowClass(const QRegularExpression &windowClassRegex);

    /**
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    Condition &setWindowState(const WindowStates &windowState);

    Condition &setNegateWindowClass(const bool &negate);
    Condition &setNegateWindowState(const bool &negate);

private:
    bool isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const;
    bool isWindowStateSubConditionSatisfied(const WindowInfo &data) const;

    std::optional<QRegularExpression> m_windowClass = std::nullopt;
    bool m_negateWindowClass = false;

    WindowStates m_windowState = WindowState::All;
    bool m_negateWindowState = false;

    friend class TestCondition;
};

}