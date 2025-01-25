#include "keysequence.h"
#include "libgestures/libgestures.h"

#include <stack>

namespace libgestures
{

bool KeySequenceGestureAction::tryExecute()
{
    if (!GestureAction::tryExecute())
        return false;

    if (m_sequence.startsWith("-") || m_sequence.startsWith("+")) {
        // Complex format
        for (const auto &command : m_sequence.split(" ")) {
            const auto action = command[0];
            const auto key = command.mid(1);

            if (!s_keyMap.contains(key.toUpper()))
                continue;

            if (action == "+")
                libgestures::input()->keyboardPress(s_keyMap.at(key));
            else if (action == "-")
                libgestures::input()->keyboardRelease(s_keyMap.at(key));
        }
    } else {
        // Simple format
        std::stack<uint32_t> keys;
        for (const auto &keyRaw : m_sequence.split("+")) {
            if (!s_keyMap.contains(keyRaw.toUpper()))
                continue;

            const auto key = s_keyMap.at(keyRaw);
            keys.push(key);
            libgestures::input()->keyboardPress(key);
        }

        while (!keys.empty()) {
            const auto key = keys.top();
            libgestures::input()->keyboardRelease(key);
            keys.pop();
        }
    }

    return true;
}

void KeySequenceGestureAction::setSequence(const QString &sequence)
{
    m_sequence = sequence;
}

}