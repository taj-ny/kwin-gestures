#include "action.h"
#include "libgestures/input.h"
#include <QString>
#include <linux/input-event-codes.h>
#include <map>

namespace libgestures
{

struct InputAction
{
    std::vector<uint32_t> keyboardPress;
    std::vector<uint32_t> keyboardRelease;

    std::vector<uint32_t> mousePress;
    std::vector<uint32_t> mouseRelease;
    QPointF mouseMoveAbsolute;
    QPointF mouseMoveRelative;
    bool mouseMoveRelativeByDelta = false;
};

/**
 * Sends input.
 *
 * @remark Requires Input::keyboardKey, Input::mouseButton, Input::mouseMoveAbsolute and Input::mouseMoveRelative to be
 * implemented.
 */
class InputGestureAction : public GestureAction
{
public:
    bool tryExecute() override;
    void setSequence(const std::vector<InputAction> &sequence);

private:
    std::vector<InputAction> m_sequence;
};

}