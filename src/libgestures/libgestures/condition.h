#pragma once

#include <QRegularExpression>
#include "windowinfoprovider.h"

namespace libgestures
{

class Condition
{
public:
    explicit Condition(std::shared_ptr<WindowInfoProvider> windowInfoProvider);

    /**
     * @return @c false if no window is currently active. @c true if all specified subconditions are satisfied or if
     * none were specified. @c true if negated and none of the specified subconditions are satisfied.
     */
    [[nodiscard]] bool isSatisfied() const;

    /**
     * @param negate Whether this condition should be satisfied only when none of the specified subconditions are.
     */
    void setNegate(const bool &negate);

    /**
     * @param windowClassRegex The pattern must not be empty, otherwise the subcondition will always be satisfied.
     * @remark Requires libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowClassRegex(const QRegularExpression &windowClassRegex);
    /**
     * @remark Requires libgestures::WindowInfoProvider to be implemented.
     */
    void setWindowState(const WindowState &windowState);
private:
    [[nodiscard]] bool isWindowClassRegexSubConditionSatisfied(const WindowInfo &data) const;
    [[nodiscard]] bool isWindowStateSubConditionSatisfied(const WindowInfo &data) const;

    const std::shared_ptr<WindowInfoProvider> m_windowInfoProvider;
    bool m_negate = false;
    std::optional<QRegularExpression> m_windowClassRegex = std::nullopt;
    std::optional<WindowState> m_windowState = std::nullopt;

    friend class TestCondition;
};

} // namespace libgestures