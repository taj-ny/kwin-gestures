#pragma once

class GestureAction
{
public:
    virtual ~GestureAction() = default;

    virtual void execute() { };

protected:
    GestureAction() = default;
};