#ifndef TDIGITALINPUT_H
#define TDIGITALINPUT_H

#include <QToolButton>

class TDigitalInput
{
public:
    TDigitalInput(QToolButton *pToolButton);

    void set(bool state) { this->state = state; pToolButton->setIcon(state ? QIcon(":/icons/on.png") : QIcon(":/icons/off.png")); }
    void toggle(void) { set(!state); }
    bool get(void) {return state; }

private:
    QToolButton *pToolButton;
    bool state;
};

#endif // TDIGITALINPUT_H
