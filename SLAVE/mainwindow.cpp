#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpprotocol.h"

#include <QTime>

#define DEFAULT_TCP_PORT    2020
#define DEFAULT_TCP_IP      "127.0.0.1"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpSlave = new QTcpSocket();

    QLabel *pAddressLabel = new QLabel();
    ui->toolBar->insertWidget(ui->actionDisconnect, pAddressLabel);
    pAddressLabel->setText(QString("IP :"));

    pAddressEdit = new QLineEdit();
    pAddressEdit->setFixedWidth(120);
    ui->toolBar->insertWidget(ui->actionDisconnect, pAddressEdit);
    pAddressEdit->setText(DEFAULT_TCP_IP);

    QLabel *pPortLabel = new QLabel();
    ui->toolBar->insertWidget(ui->actionDisconnect, pPortLabel);
    pPortLabel->setText(QString("Port :"));

    pPortEdit = new QLineEdit();
    pPortEdit->setFixedWidth(60);
    ui->toolBar->insertWidget(ui->actionDisconnect, pPortEdit);
    pPortEdit->setText(QString().setNum(DEFAULT_TCP_PORT));

    // // Array per gli Input
    digitalInput << new TDigitalInput(ui->input1);
    digitalInput << new TDigitalInput(ui->input2);
    digitalInput << new TDigitalInput(ui->input3);
    digitalInput << new TDigitalInput(ui->input4);
    digitalInput << new TDigitalInput(ui->input5);
    digitalInput << new TDigitalInput(ui->input6);
    digitalInput << new TDigitalInput(ui->input7);
    digitalInput << new TDigitalInput(ui->input8);
    digitalInput << new TDigitalInput(ui->input9);
    digitalInput << new TDigitalInput(ui->input10);

    // // Array per gli Output
    outputButtons << ui->output1 << ui->output2 << ui->output3 << ui->output4 << ui->output5 << ui->output6 << ui->output7 << ui->output8 << ui->output9 << ui->output10;
    for (int i = 0; i < outputButtons.size(); ++i)
        outputButtons[i]->setIcon(QIcon(":/icons/off.png"));

    // Array per gli Analog
    analogButtons << ui->analog1 << ui->analog2 << ui->analog3 << ui->analog4 << ui->analog5 << ui->analog6 << ui->analog7 << ui->analog8 << ui->analog9 << ui->analog10;

    connect(ui->actionConnect, &QAction::triggered, this, &MainWindow::start);
    connect(ui->actionDisconnect, &QAction::triggered, this, &MainWindow::stop);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clear);

    QObject::connect(tcpSlave, &QTcpSocket::connected, this, &MainWindow::connection);
    QObject::connect(tcpSlave, &QTcpSocket::disconnected, this, &MainWindow::disconnection);
    QObject::connect(tcpSlave, &QTcpSocket::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start(void)
{
    tcpSlave->connectToHost(pAddressEdit->text(), pPortEdit->text().toShort());
}

void MainWindow::stop(void)
{
    tcpSlave->disconnectFromHost();
}

void MainWindow::readData(void)
{
    TcpProtocol tcpProtocol;
    QByteArray data = tcpSlave->readAll();

    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message received " + QString(data));
    if (tcpProtocol.fromCommand((char *)data.toStdString().c_str()))
    {
        if (tcpProtocol.getTarget() == TcpProtocol::eTargetDin)
        {
            char *message = tcpProtocol.toAnswer(digitalInput[tcpProtocol.getIdx()-1]->get() ? TcpProtocol::eStateOn : TcpProtocol::eStateOff);
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
            tcpSlave->write(message);
        }
        else if (tcpProtocol.getTarget() == TcpProtocol::eTargetDout)
        {
            outputButtons[tcpProtocol.getIdx()-1]->setIcon(tcpProtocol.getState() == TcpProtocol::eStateOn ? QIcon(":/icons/on.png") :  QIcon(":/icons/off.png"));
            char *message = tcpProtocol.toAnswer(tcpProtocol.getState());
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
            tcpSlave->write(message);
        }
        else if (tcpProtocol.getTarget() == TcpProtocol::eTargetAnalog)
        {
            char *message = tcpProtocol.toAnswer(analogButtons[tcpProtocol.getIdx()-1]->text().toInt());
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
            tcpSlave->write(message);
        }
    }
}

void MainWindow::clear(void)
{
    ui->plainTextEditLog->clear();
}

void MainWindow::connection(void)
{
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Connected");
}

void MainWindow::disconnection(void)
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Disconnected");
}

void MainWindow::on_input_toggled(int idx, bool checked)
{
    digitalInput[idx]->set(checked);
}

void MainWindow::on_input1_toggled(bool checked)
{
    on_input_toggled(0, checked);
}

void MainWindow::on_input2_toggled(bool checked)
{
    on_input_toggled(1, checked);
}

void MainWindow::on_input3_toggled(bool checked)
{
    on_input_toggled(2, checked);
}

void MainWindow::on_input4_toggled(bool checked)
{
    on_input_toggled(3, checked);
}

void MainWindow::on_input5_toggled(bool checked)
{
    on_input_toggled(4, checked);
}

void MainWindow::on_input6_toggled(bool checked)
{
    on_input_toggled(5, checked);
}

void MainWindow::on_input7_toggled(bool checked)
{
    on_input_toggled(6, checked);
}

void MainWindow::on_input8_toggled(bool checked)
{
    on_input_toggled(7, checked);
}

void MainWindow::on_input9_toggled(bool checked)
{
    on_input_toggled(8, checked);
}

void MainWindow::on_input10_toggled(bool checked)
{
    on_input_toggled(9, checked);
}

