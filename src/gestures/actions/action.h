#pragma once

class GestureAction
{
public:
    virtual void execute() { };

protected:
    GestureAction() = default;
};