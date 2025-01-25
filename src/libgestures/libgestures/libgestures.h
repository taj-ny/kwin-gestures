#pragma once

#include "input.h"
#include "windowinfoprovider.h"

namespace libgestures
{

class libgestures
{
public:
    static Input *input();
    static WindowInfoProvider *windowInfoProvider();
    static void reset();

    static void setInput(Input *input);
    static void setWindowInfoProvider(WindowInfoProvider *windowInfoProvider);

private:
    static std::unique_ptr<Input> m_input;
    static std::unique_ptr<WindowInfoProvider> m_windowInfoProvider;
};

}