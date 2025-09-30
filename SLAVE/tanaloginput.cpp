#include "tanaloginput.h"

TAnalogInput::TAnalogInput(QLineEdit *pLineEdit) :
    pLineEdit(pLineEdit),
    val(0)
{
    pLineEdit->setText(QString::number(0));
}
