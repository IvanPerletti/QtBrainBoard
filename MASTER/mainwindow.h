#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>

#include "tcpmaster.h"

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
    void listen();
    void stop();
    void clear();

    void on_pushButtonGetState_clicked();

    void on_pushButtonSetState_clicked();

    void on_pushButtonGetValue_clicked();

    void on_pushButtonOpenCAN_clicked();

    void on_pushButtonCloseCAN_clicked();

    void on_pushButtonSpeedCAN_clicked();

    void on_pushButtonFilterCAN_clicked();

    void on_pushButtonWriteCAN_clicked();

    void on_pushButtonReadCAN_clicked();

public slots:
    void newConnection(QTcpSocket *client);
    void disconnection(QTcpSocket *client);
    void readSocket(QTcpSocket *client, const QByteArray &data);

private:
    Ui::MainWindow *ui;

    TcpMaster *tcpMaster;
    QLineEdit *pPortEdit;

    QVector<QRadioButton*> inputButtons;
    QVector<QRadioButton*> outputButtons;
    QVector<QRadioButton*> analogButtons;
    QVector<QLabel*> analogValues;
};
#endif // MAINWINDOW_H
