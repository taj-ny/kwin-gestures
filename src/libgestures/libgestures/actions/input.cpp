#include "libgestures/libgestures/actions/input.h"

namespace libgestures
{

InputGestureAction::InputGestureAction(const std::vector<InputAction> &sequence)
    : m_sequence(sequence)
{
}

bool InputGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute()) {
        return false;
    }

    const auto input = libgestures::Input::implementation();
    for (const auto &action : m_sequence) {
        for (const auto &key : action.keyboardPress) {
            input->keyboardKey(key, true);
        }
        for (const auto &key : action.keyboardRelease) {
            input->keyboardKey(key, false);
        }

        for (const auto &button : action.mousePress) {
            input->mouseButton(button, true);
        }
        for (const auto &button : action.mouseRelease) {
            input->mouseButton(button, false);
        }

        if (!action.mouseMoveAbsolute.isNull()) {
            input->mouseMoveAbsolute(action.mouseMoveAbsolute);
        }
        if (!action.mouseMoveRelative.isNull()) {
            input->mouseMoveRelative(action.mouseMoveRelative);
        }
        if (action.mouseMoveRelativeByDelta) {
            input->mouseMoveRelative(m_currentDeltaPointMultiplied);
        }
    }

    return true;
}

}