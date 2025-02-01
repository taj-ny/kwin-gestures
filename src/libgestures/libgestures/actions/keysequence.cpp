#include "keysequence.h"
#include "libgestures/libgestures.h"

#include <stack>

namespace libgestures
{

bool KeySequenceGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute())
        return false;

    for (const auto &command : m_sequence.split(" ")) {
        if (command.startsWith("+") || command.startsWith("-")) {
            const auto action = command[0];
            const auto key = command.mid(1);

            if (!s_keyMap.contains(key)) {
                continue;
            }

            if (action == '+')
                libgestures::input()->keyboardKey(s_keyMap.at(key), true);
            else if (action == '-')
                libgestures::input()->keyboardKey(s_keyMap.at(key), false);
        } else {
            std::stack<uint32_t> keys;
            for (const auto &keyRaw : command.split("+")) {
                if (!s_keyMap.contains(keyRaw))
                    continue;

                const auto key = s_keyMap.at(keyRaw);
                keys.push(key);
                libgestures::input()->keyboardKey(key, true);
            }

            while (!keys.empty()) {
                const auto key = keys.top();
                libgestures::input()->keyboardKey(key, false);
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