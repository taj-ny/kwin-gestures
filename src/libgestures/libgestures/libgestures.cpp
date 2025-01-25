#include "libgestures.h"

namespace libgestures
{

std::unique_ptr<Input> libgestures::m_input = std::make_unique<Input>();
std::unique_ptr<WindowInfoProvider> libgestures::m_windowInfoProvider = std::make_unique<WindowInfoProvider>();

Input *libgestures::input()
{
    return m_input.get();
}

WindowInfoProvider *libgestures::windowInfoProvider()
{
    return m_windowInfoProvider.get();
}

void libgestures::reset()
{
    m_input.reset(nullptr);
    m_windowInfoProvider.reset(nullptr);
}

void libgestures::setInput(Input *input)
{
    m_input.reset(input);
}

void libgestures::setWindowInfoProvider(WindowInfoProvider *windowInfoProvider)
{
    m_windowInfoProvider.reset(windowInfoProvider);
}

}