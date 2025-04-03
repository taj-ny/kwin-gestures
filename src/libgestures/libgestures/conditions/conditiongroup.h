#include "libgestures/conditions/condition.h"

#include <memory>
#include <vector>

namespace libgestures
{

enum class ConditionGroupMode
{
    All,
    Any,
    None
};

class ConditionGroup : public ConditionV2
{
public:
    bool satisfied() const override;

    void add(const std::shared_ptr<ConditionV2> &condition);
    void setMode(const ConditionGroupMode &mode);

protected:
    std::vector<std::shared_ptr<ConditionV2>> m_conditions;
    ConditionGroupMode m_mode = ConditionGroupMode::All;
};

}