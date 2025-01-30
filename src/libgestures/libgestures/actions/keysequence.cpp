#include "keysequence.h"

#include <stack>

namespace libgestures
{

bool KeySequenceGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute())
        return false;

    const auto input = libgestures::Input::implementation();
    for (const auto &command : m_sequence.split(" ")) {
        if (command.startsWith("+") || command.startsWith("-")) {
            const auto action = command[0];
            const auto key = command.mid(1);

            if (!s_keyMap.contains(key)) {
                continue;
            }

            if (action == '+')
                input->keyboardPress(s_keyMap.at(key));
            else if (action == '-')
                input->keyboardRelease(s_keyMap.at(key));
        } else {
            std::stack<uint32_t> keys;
            for (const auto &keyRaw : command.split("+")) {
                if (!s_keyMap.contains(keyRaw))
                    continue;

                const auto key = s_keyMap.at(keyRaw);
                keys.push(key);
                input->keyboardPress(key);
            }

            while (!keys.empty()) {
                const auto key = keys.top();
                input->keyboardRelease(key);
                keys.pop();
            }
        }
    }

    return true;
}

void KeySequenceGestureAction::setSequence(const QString &sequence)
{
    m_sequence = sequence.toUpper();
}

}