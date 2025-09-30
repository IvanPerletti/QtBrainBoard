#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>

#include "tdigitalinput.h"
#include "tdigitaloutput.h"
#include "tanaloginput.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void start();
    void stop();
    void clear();

    void on_input1_toggled(bool checked);

    void on_input2_toggled(bool checked);

    void on_input3_toggled(bool checked);

    void on_input4_toggled(bool checked);

    void on_input5_toggled(bool checked);

    void on_input6_toggled(bool checked);

    void on_input7_toggled(bool checked);

    void on_input8_toggled(bool checked);

    void on_input9_toggled(bool checked);

    void on_input10_toggled(bool checked);

    void on_analog1_editingFinished();

    void on_analog2_editingFinished();

    void on_analog3_editingFinished();

    void on_analog4_editingFinished();

    void on_analog5_editingFinished();

    void on_analog6_editingFinished();

    void on_analog7_editingFinished();

    void on_analog8_editingFinished();

    void on_analog9_editingFinished();

    void on_analog10_editingFinished();

public slots:
    void connection(void);
    void disconnection(void);
    void readData(void);

private:
    Ui::MainWindow *ui;

    QLineEdit *pAddressEdit;
    QLineEdit *pPortEdit;
    QTcpSocket *tcpSlave;

    QVector<TDigitalInput *> digitalInput;
    QVector<TDigitalOutput*> digitalOutput;
    QVector<TAnalogInput*> analogInput;

    void on_input_toggled(int idx, bool checked);
    void on_analog_editingFinished(int idx, int val);
};
#endif // MAINWINDOW_H
