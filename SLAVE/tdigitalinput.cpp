#include "tdigitalinput.h"

TDigitalInput::TDigitalInput(QToolButton *pToolButton) :
    pToolButton(pToolButton),
    state(false)
{
    pToolButton->setIcon(QIcon(":/icons/off.png"));
}
