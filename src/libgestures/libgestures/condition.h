#pragma once

#include <QRegularExpression>
#include "windowinfoprovider.h"

namespace libgestures
{

class Condition
{
public:
    /**
     * @return @c false if no window is currently active. @c true if all specified subconditions are satisfied or if
     * none were specified. @c true if negated and none of the specified subconditions are satisfied.
     */
    [[nodiscard]] bool isSatisfied() const;

    /**
     * @param windowClassRegex The pattern must not be empty, otherwise the subcondition will always be satisfied.
     * @remark Requires libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowClass(const QRegularExpression &windowClassRegex);
    /**
     * @remark Requires libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowState(const WindowStates &windowState);

    void setNegateWindowClass(const bool &negate);
    void setNegateWindowState(const bool &negate);
private:
    [[nodiscard]] bool isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const;
    [[nodiscard]] bool isWindowStateSubConditionSatisfied(const WindowInfo &data) const;

    std::optional<QRegularExpression> m_windowClass = std::nullopt;
    bool m_negateWindowClass = false;

    WindowStates m_windowState = WindowState::All;
    bool m_negateWindowState = false;

    friend class TestCondition;
};

}