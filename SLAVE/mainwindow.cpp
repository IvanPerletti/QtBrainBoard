#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TDigitalPort.h"
#include "TAnalogPort.h"
#include "ICan.h"

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
    inputButtons << ui->input1;
    inputButtons << ui->input2;
    inputButtons << ui->input3;
    inputButtons << ui->input4;
    inputButtons << ui->input5;
    inputButtons << ui->input6;
    inputButtons << ui->input7;
    inputButtons << ui->input8;
    inputButtons << ui->input9;
    inputButtons << ui->input10;
    for (int i = 0; i < inputButtons.size(); ++i)
        inputButtons[i]->setIcon(QIcon(":/icons/off.png"));

    // // Array per gli Output
    outputButtons << ui->output1;
    outputButtons << ui->output2;
    outputButtons << ui->output3;
    outputButtons << ui->output4;
    outputButtons << ui->output5;
    outputButtons << ui->output6;
    outputButtons << ui->output7;
    outputButtons << ui->output8;
    outputButtons << ui->output9;
    outputButtons << ui->output10;
    for (int i = 0; i < outputButtons.size(); ++i)
        outputButtons[i]->setIcon(QIcon(":/icons/off.png"));

    // Array per gli Analog
    analogEdits << ui->analog1;
    analogEdits << ui->analog2;
    analogEdits << ui->analog3;
    analogEdits << ui->analog4;
    analogEdits << ui->analog5;
    analogEdits << ui->analog6;
    analogEdits << ui->analog7;
    analogEdits << ui->analog8;
    analogEdits << ui->analog9;
    analogEdits << ui->analog10;
    for (int i = 0; i < analogEdits.size(); ++i)
        analogEdits[i]->setText(QString::number(0));

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

    QByteArray data = tcpSlave->readAll();

    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message received " + QString(data).chopped(1));
    parseCommand((char *)data.toStdString().c_str());
}

void MainWindow::parseCommand(char *message)
{
    if (tcpProtocolSlave.fromCommand(message))
    {
        if (tcpProtocolSlave.getTarget() == TcpProtocol::eTargetDin)
        {
            char *message = tcpProtocolSlave.toAnswer(digitalPort.check((enumDigitalIn)tcpProtocolSlave.getIdx()) == HIGH ? TcpProtocol::eStateOn : TcpProtocol::eStateOff);
#ifndef STM32F4XX
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
            tcpSlave->write(message);
#else
            pEth->write(message, (int)strlen(message));
#endif
        }
        else if (tcpProtocolSlave.getTarget() == TcpProtocol::eTargetDout)
        {
#ifndef STM32F4XX
            outputButtons[tcpProtocolSlave.getIdx()-1]->setIcon(tcpProtocolSlave.getState() == TcpProtocol::eStateOn ? QIcon(":/icons/on.png") : QIcon(":/icons/off.png"));
#endif
            if (tcpProtocolSlave.getState() == TcpProtocol::eStateOn)
                digitalPort.setNow((enumDigitalOut)tcpProtocolSlave.getIdx());
            else
                digitalPort.resetNow((enumDigitalOut)tcpProtocolSlave.getIdx());
            char *message = tcpProtocolSlave.toAnswer(digitalPort.check((enumDigitalOut)tcpProtocolSlave.getIdx()) == HIGH ? TcpProtocol::eStateOn : TcpProtocol::eStateOff);
#ifndef STM32F4XX
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
            tcpSlave->write(message);
#else
            pEth->write(message, (int)strlen(message));
#endif
        }
        else if (tcpProtocolSlave.getTarget() == TcpProtocol::eTargetAnalog)
        {
            char *message = tcpProtocolSlave.toAnswer(tAnalogPort.read((enumAnalogPort)(tcpProtocolSlave.getIdx()-1)));
#ifndef STM32F4XX
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
            tcpSlave->write(message);
#else
            pEth->write(message, (int)strlen(message));
#endif
        }
        else if (tcpProtocolSlave.getTarget() == TcpProtocol::eTargetCAN)
        {
            char *message = NULL;

            if (tcpProtocolSlave.getCommand() == TcpProtocol::eCmdOpen)
            {
                iCan.open(0);
                message = tcpProtocolSlave.toAnswer(TcpProtocol::eStateOpened);
            }
            else if (tcpProtocolSlave.getCommand() == TcpProtocol::eCmdClose)
            {
                iCan.close(0);
                message = tcpProtocolSlave.toAnswer(TcpProtocol::eStateClosed);
            }
            else if (tcpProtocolSlave.getCommand() == TcpProtocol::eCmdSet)
            {
                if (tcpProtocolSlave.getParam() == TcpProtocol::eParamSpeed)
                {
                    int speed = strtol(tcpProtocolSlave.getParams(0), NULL, 10);
                    switch (speed)
                    {
                    case 125:
                        iCan.setSpeed(ICan::CAN_BUS_SPEED_125);
                        break;
                    case 250:
                        iCan.setSpeed(ICan::CAN_BUS_SPEED_250);
                        break;
                    case 500:
                        iCan.setSpeed(ICan::CAN_BUS_SPEED_500);
                        break;
                    default:
                        ; // send error answer
                    }
                    message = tcpProtocolSlave.toAnswer(tcpProtocolSlave.getParam(), (int)iCan.getSpeed());
                }
                else if (tcpProtocolSlave.getParam() == TcpProtocol::eParamFilter)
                {
                    bool ret = false;
                    uint16_t filters[4];

                    for (int ind=0; ind<tcpProtocolSlave.getNParams(); ind++)
                        filters[ind] = (uint16_t)strtol(tcpProtocolSlave.getParams(ind), NULL, 10);
                    switch (tcpProtocolSlave.getNParams())
                    {
                    case 1:
                        ret = iCan.registerFilter(0, filters[0]);
                        break;
                    case 2:
                        ret = iCan.registerFilter(0, filters[0], filters[1]);
                        break;
                    case 3:
                        ret = iCan.registerFilter(0, filters[0], filters[1], filters[2]);
                        break;
                    case 4:
                        ret = iCan.registerFilter(0, filters[0], filters[1], filters[2], filters[3]);
                        break;
                    }
                    if (ret)
                        message = tcpProtocolSlave.toAnswer(tcpProtocolSlave.getParam(), tcpProtocolSlave.getNParams());
                    else
                        ; // send error answer
                }
            }
            else if (tcpProtocolSlave.getCommand() == TcpProtocol::eCmdSend)
            {
                if (tcpProtocolSlave.getParam() == TcpProtocol::eParamData)
                {
                    bool ret = false;
                    CanTxMsg canTxMsg;

                    if (tcpProtocolSlave.getNParams() > 1)
                    {
                        canTxMsg.StdId = (uint32_t)strtol(tcpProtocolSlave.getParams(0), NULL, 16);
                        canTxMsg.IDE = 0;
                        canTxMsg.DLC = tcpProtocolSlave.getNParams() - 1;
                        for (int ind=0; ind<tcpProtocolSlave.getNParams() - 1; ind++)
                            canTxMsg.Data[ind] = (uint16_t)strtol(tcpProtocolSlave.getParams(ind+1), NULL, 16);
                        iCan.write(&canTxMsg);
                        ret = true;
                    }
                    if (ret)
                        message = tcpProtocolSlave.toAnswer(tcpProtocolSlave.getParam(), tcpProtocolSlave.getNParams());
                    else
                        ; // send error answer
                }

            }
            if (message)
            {
#ifndef STM32F4XX
                ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message));
                tcpSlave->write(message);
#else
                pEth->write(message, (int)strlen(message));
#endif
            }
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
    digitalPort.updateIN((enumDigitalIn)idx, checked ? HIGH : LOW);
    inputButtons[idx]->setIcon(checked ? QIcon(":/icons/on.png") : QIcon(":/icons/off.png"));
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

void MainWindow::on_analog_editingFinished(int idx, int val)
{
    tAnalogPort.write((enumAnalogPort)idx, (long)val);
}

void MainWindow::on_analog1_editingFinished()
{
    on_analog_editingFinished(0, ui->analog1->text().toInt());
}

void MainWindow::on_analog2_editingFinished()
{
    on_analog_editingFinished(1, ui->analog2->text().toInt());
}

void MainWindow::on_analog3_editingFinished()
{
    on_analog_editingFinished(2, ui->analog3->text().toInt());
}

void MainWindow::on_analog4_editingFinished()
{
    on_analog_editingFinished(3, ui->analog4->text().toInt());
}

void MainWindow::on_analog5_editingFinished()
{
    on_analog_editingFinished(4, ui->analog5->text().toInt());
}

void MainWindow::on_analog6_editingFinished()
{
    on_analog_editingFinished(5, ui->analog6->text().toInt());
}

void MainWindow::on_analog7_editingFinished()
{
    on_analog_editingFinished(6, ui->analog7->text().toInt());
}

void MainWindow::on_analog8_editingFinished()
{
    on_analog_editingFinished(7, ui->analog8->text().toInt());
}

void MainWindow::on_analog9_editingFinished()
{
    on_analog_editingFinished(8, ui->analog9->text().toInt());
}

void MainWindow::on_analog10_editingFinished()
{
    on_analog_editingFinished(9, ui->analog10->text().toInt());
}

