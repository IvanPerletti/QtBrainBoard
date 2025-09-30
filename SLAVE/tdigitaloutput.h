#ifndef TDIGITALOUTPUT_H
#define TDIGITALOUTPUT_H

#include <QToolButton>

class TDigitalOutput
{
public:
    TDigitalOutput(QToolButton *pToolButton);

    void set(bool state) { this->state = state; pToolButton->setIcon(state ? QIcon(":/icons/on.png") : QIcon(":/icons/off.png")); }
    void toggle(void) { set(!state); }
    bool get(void) {return state; }

private:
    QToolButton *pToolButton;
    bool state;
};

#endif // TDIGITALOUTPUT_H
