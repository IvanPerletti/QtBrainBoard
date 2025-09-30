#include "tdigitaloutput.h"

TDigitalOutput::TDigitalOutput(QToolButton *pToolButton) :
    pToolButton(pToolButton),
    state(false)
{
    pToolButton->setIcon(QIcon(":/icons/off.png"));
}
