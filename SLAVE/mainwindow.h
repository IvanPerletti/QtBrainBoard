#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>

#include "tdigitalinput.h"

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

public slots:
    void connection(void);
    void disconnection(void);
    void readData(void);

private:
    Ui::MainWindow *ui;

    QLineEdit *pAddressEdit;
    QLineEdit *pPortEdit;
    QTcpSocket *tcpSlave;

    QVector<TDigitalInput *>digitalInput;
    QVector<QToolButton*> outputButtons;
    QVector<QLineEdit*> analogButtons;

    void on_input_toggled(int idx, bool checked);
};
#endif // MAINWINDOW_H
