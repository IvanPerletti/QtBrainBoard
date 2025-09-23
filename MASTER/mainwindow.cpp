#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpprotocol.h"

#include <QTime>
#include <QLineEdit>

#define DEFAULT_TCP_PORT    2020

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpMaster = new TcpMaster();

    QLabel *pPortLabel = new QLabel();
    ui->toolBar->insertWidget(ui->actionStop, pPortLabel);
    pPortLabel->setText(QString("Port :"));

    pPortEdit = new QLineEdit();
    pPortEdit->setFixedWidth(60);
    ui->toolBar->insertWidget(ui->actionStop, pPortEdit);
    pPortEdit->setText(QString().setNum(DEFAULT_TCP_PORT));

    // Array per gli Input
    inputButtons << ui->input1 << ui->input2 << ui->input3 << ui->input4 << ui->input5 << ui->input6 << ui->input7 << ui->input8 << ui->input9 << ui->input10;

    for (int i = 0; i < inputButtons.size(); ++i)
        inputButtons[i]->setIcon(QIcon(":/icons/off.png"));
    inputButtons[0]->setChecked(true);

    // Array per gli Output
    outputButtons << ui->output1 << ui->output2 << ui->output3 << ui->output4 << ui->output5 << ui->output6 << ui->output7 << ui->output8 << ui->output9 << ui->output10;

    for (int i = 0; i < outputButtons.size(); ++i)
        outputButtons[i]->setIcon(QIcon(":/icons/off.png"));
    outputButtons[0]->setChecked(true);

    // Array per gli Analog
    analogButtons << ui->analog1 << ui->analog2 << ui->analog3 << ui->analog4 << ui->analog5 << ui->analog6 << ui->analog7 << ui->analog8 << ui->analog9 << ui->analog10;
    analogValues << ui->analogValue1 << ui->analogValue2 << ui->analogValue3 << ui->analogValue4 << ui->analogValue5 << ui->analogValue6 << ui->analogValue7 << ui->analogValue8 << ui->analogValue9 << ui->analogValue10;
    analogButtons[0]->setChecked(true);

    connect(ui->actionListen, &QAction::triggered, this, &MainWindow::listen);
    connect(ui->actionStop, &QAction::triggered, this, &MainWindow::stop);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clear);

    ui->pushButtonGetState->setEnabled(false);
    ui->pushButtonSetState->setEnabled(false);
    ui->pushButtonGetValue->setEnabled(false);

    QObject::connect(tcpMaster, &TcpMaster::clientConnected, this, &MainWindow::newConnection);
    QObject::connect(tcpMaster, &TcpMaster::dataReceived, this, &MainWindow::readSocket);
    QObject::connect(tcpMaster, &TcpMaster::clientDisconnected, this, &MainWindow::disconnection);
}

MainWindow::~MainWindow()
{
    if (tcpMaster)
        delete tcpMaster;

    delete ui;
}

void MainWindow::newConnection(QTcpSocket *client)
{
    ui->pushButtonGetState->setEnabled(true);
    ui->pushButtonSetState->setEnabled(true);
    ui->pushButtonGetValue->setEnabled(true);
    ui->statusbar->showMessage("Client connected");
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Connection from " + client->peerAddress().toString());
}

void MainWindow::disconnection(QTcpSocket *client)
{
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Disconnection from " + client->peerAddress().toString());
    ui->pushButtonGetState->setEnabled(false);
    ui->pushButtonSetState->setEnabled(false);
    ui->pushButtonGetValue->setEnabled(false);
    ui->statusbar->showMessage("Client disconnected");
}

void MainWindow::listen(void)
{
    if (tcpMaster->startServer(pPortEdit->text().toShort()))
    {
        ui->actionListen->setEnabled(false);
        ui->actionStop->setEnabled(true);
        ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Start listening");
    }
}

void MainWindow::stop(void)
{
    tcpMaster->stopServer();
    ui->actionListen->setEnabled(true);
    ui->actionStop->setEnabled(false);
    ui->pushButtonGetState->setEnabled(false);
    ui->pushButtonSetState->setEnabled(false);
    ui->pushButtonGetValue->setEnabled(false);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Stop listening");
}

void MainWindow::readSocket(QTcpSocket *client, const QByteArray &data)
{
    TcpProtocol tcpProtocol;

    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message received " + QString(data));
    if (tcpProtocol.fromAnswer((char *)data.toStdString().c_str()))
    {
        if (tcpProtocol.getTarget() == TcpProtocol::eTargetDin)
        {
            inputButtons[tcpProtocol.getIdx()-1]->setIcon(tcpProtocol.getState() == TcpProtocol::eStateOn ? QIcon(":/icons/on.png") :  QIcon(":/icons/off.png"));
        }
        else if (tcpProtocol.getTarget() == TcpProtocol::eTargetDout)
        {
            outputButtons[tcpProtocol.getIdx()-1]->setIcon(tcpProtocol.getState() == TcpProtocol::eStateOn ? QIcon(":/icons/on.png") :  QIcon(":/icons/off.png"));
        }
        else if (tcpProtocol.getTarget() == TcpProtocol::eTargetAnalog)
        {
            analogValues[tcpProtocol.getIdx()-1]->setText(QString::number(tcpProtocol.getValue()));
        }
    }
}

void MainWindow::clear(void)
{

}

void MainWindow::configure(void)
{

}

void MainWindow::on_pushButtonGetState_clicked()
{
    int i;

    for (i = 0; i < inputButtons.size(); ++i) {
        if (inputButtons[i]->isChecked())
            break;
    }

    tcpMaster->sendToClient(QByteArray(TcpProtocol(TcpProtocol::eCmdGet, TcpProtocol::eTargetDin).toCommand(i + 1)));
}


void MainWindow::on_pushButtonSetState_clicked()
{
    int i;

    for (i = 0; i < outputButtons.size(); ++i) {
        if (outputButtons[i]->isChecked())
            break;
    }

    TcpProtocol::EStateType state = ui->radioOn->isChecked() ? TcpProtocol::eStateOn : TcpProtocol::eStateOff;
    tcpMaster->sendToClient(QByteArray(TcpProtocol(TcpProtocol::eCmdSet, TcpProtocol::eTargetDout).toCommand(i + 1, state)));
}


void MainWindow::on_pushButtonGetValue_clicked()
{
    int i;

    for (i = 0; i < analogButtons.size(); ++i) {
        if (analogButtons[i]->isChecked())
            break;
    }

    tcpMaster->sendToClient(QByteArray(TcpProtocol(TcpProtocol::eCmdGet, TcpProtocol::eTargetAnalog).toCommand(i + 1)));
}

