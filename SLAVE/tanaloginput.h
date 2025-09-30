#ifndef TANALOGINPUT_H
#define TANALOGINPUT_H

#include <QLineEdit>

class TAnalogInput
{
public:
    TAnalogInput(QLineEdit *pLineEdit);

    void set(int val) { this->val = val; }
    int get(void) {return val; }

private:
    QLineEdit *pLineEdit;
    int val;
};

#endif // TANALOGINPUT_H
