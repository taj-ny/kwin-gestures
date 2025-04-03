#pragma once

namespace libgestures
{

class ConditionV2
{
public:
    virtual ~ConditionV2() = default;

    virtual bool satisfied() const { return false; };

protected:
    ConditionV2();
};

}