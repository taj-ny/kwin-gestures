#pragma once

#include "libgestures/libgestures/windowinfoprovider.h"
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
    virtual bool isSatisfied() const;

    void setRequired(const bool &required);

    /**
     * @param required Whether this condition is required. All required conditions must be met. If there are any
     * non-required conditions, at least one must be met as well.
     */
    const bool &required() const;

    /**
     * @param windowClassRegex If empty, the subcondition will always be satisfied.
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowClass(const QRegularExpression &windowClassRegex);

    /**
     * @remark Requires @c libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowState(const WindowStates &windowState);

    void setNegateWindowClass(const bool &negate);
    void setNegateWindowState(const bool &negate);

private:
    bool isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const;
    bool isWindowStateSubConditionSatisfied(const WindowInfo &data) const;

    std::optional<QRegularExpression> m_windowClass = std::nullopt;
    bool m_negateWindowClass = false;

    WindowStates m_windowState = WindowState::All;
    bool m_negateWindowState = false;

    bool m_required = false;

    friend class TestCondition;
};

}