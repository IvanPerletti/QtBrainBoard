#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "..\Lib\tcpprotocol.h"

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
    ui->pushButtonOpenCAN->setEnabled(false);
    ui->pushButtonCloseCAN->setEnabled(false);
    ui->pushButtonFilterCAN->setEnabled(false);
    ui->pushButtonSpeedCAN->setEnabled(false);
    ui->pushButtonWriteCAN->setEnabled(false);
    ui->pushButtonReadCAN->setEnabled(false);

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
    ui->pushButtonOpenCAN->setEnabled(true);
    ui->pushButtonCloseCAN->setEnabled(true);
    ui->pushButtonFilterCAN->setEnabled(true);
    ui->pushButtonSpeedCAN->setEnabled(true);
    ui->pushButtonWriteCAN->setEnabled(true);
    ui->pushButtonReadCAN->setEnabled(true);
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
    TcpProtocolMaster tcpProtocol;

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
        else if (tcpProtocol.getTarget() == TcpProtocol::eTargetCAN)
        {
            if (tcpProtocol.getCommand() == TcpProtocol::eCmdReceive)
            {
                ui->lineEditRxIdCAN->setText(QString(tcpProtocol.getParams(0)));
                for (int ind=1; ind<tcpProtocol.getNParams(); ind++)
                {
                    QLineEdit *editTxData;
                    switch (ind)
                    {
                    case 0: editTxData = ui->lineEditTxData1CAN; break;
                    case 1: editTxData = ui->lineEditTxData2CAN; break;
                    case 2: editTxData = ui->lineEditTxData3CAN; break;
                    case 3: editTxData = ui->lineEditTxData4CAN; break;
                    case 4: editTxData = ui->lineEditTxData5CAN; break;
                    case 5: editTxData = ui->lineEditTxData6CAN; break;
                    case 6: editTxData = ui->lineEditTxData7CAN; break;
                    case 7: editTxData = ui->lineEditTxData8CAN; break;
                    }
                    editTxData->setText(QString(tcpProtocol.getParams(ind)));
                }
            }
        }
    }
}

void MainWindow::clear(void)
{
    ui->plainTextEditLog->clear();
}

void MainWindow::on_pushButtonGetState_clicked()
{
    int i;

    for (i = 0; i < inputButtons.size(); ++i) {
        if (inputButtons[i]->isChecked())
            break;
    }

    QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdGet, TcpProtocol::eTargetDin, i + 1));
    tcpMaster->sendToClient(message);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
}


void MainWindow::on_pushButtonSetState_clicked()
{
    int i;

    for (i = 0; i < outputButtons.size(); ++i) {
        if (outputButtons[i]->isChecked())
            break;
    }
    TcpProtocol::EStateType state = ui->radioOn->isChecked() ? TcpProtocol::eStateOn : TcpProtocol::eStateOff;

    QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdSet, TcpProtocol::eTargetDout, i + 1, state));
    tcpMaster->sendToClient(message);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
}


void MainWindow::on_pushButtonGetValue_clicked()
{
    int i;

    for (i = 0; i < analogButtons.size(); ++i) {
        if (analogButtons[i]->isChecked())
            break;
    }

    QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdGet, TcpProtocol::eTargetAnalog, i + 1));
    tcpMaster->sendToClient(message);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
}


void MainWindow::on_pushButtonOpenCAN_clicked()
{
    QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdOpen, TcpProtocol::eTargetCAN, 1));
    tcpMaster->sendToClient(message);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
}


void MainWindow::on_pushButtonCloseCAN_clicked()
{
    QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdClose, TcpProtocol::eTargetCAN, 1));
    tcpMaster->sendToClient(message);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
}


void MainWindow::on_pushButtonSpeedCAN_clicked()
{
    char str_param[MAX_LEN_PARAM+1];
    char *params[1];

    if (ui->comboBoxSpeedCAN->currentText().length() > 0)
    {
        strncpy(str_param, ui->comboBoxSpeedCAN->currentText().toStdString().c_str(), MAX_LEN_PARAM);
        str_param[MAX_LEN_PARAM] = '\0';
        params[0] = str_param;
    }
    QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdSet, TcpProtocol::eTargetCAN, 1, TcpProtocol::eParamSpeed, 1, params));
    tcpMaster->sendToClient(message);
    ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
}


void MainWindow::on_pushButtonFilterCAN_clicked()
{
    int nparams = 0;
    char str_param[4][MAX_LEN_PARAM+1];
    char *params[4];

    if (ui->lineEditFilter1CAN->text().length() > 0)
    {
        strncpy(str_param[nparams], ui->lineEditFilter1CAN->text().toStdString().c_str(), MAX_LEN_PARAM);
        str_param[nparams][MAX_LEN_PARAM] = '\0';
        params[nparams] = str_param[nparams];
        ++nparams;
    }
    if (ui->lineEditFilter2CAN->text().length() > 0)
    {
        strncpy(str_param[nparams], ui->lineEditFilter2CAN->text().toStdString().c_str(), MAX_LEN_PARAM);
        str_param[nparams][MAX_LEN_PARAM] = '\0';
        params[nparams] = str_param[nparams];
        ++nparams;
    }
    if (ui->lineEditFilter3CAN->text().length() > 0)
    {
        strncpy(str_param[nparams], ui->lineEditFilter3CAN->text().toStdString().c_str(), MAX_LEN_PARAM);
        str_param[nparams][MAX_LEN_PARAM] = '\0';
        params[nparams] = str_param[nparams];
        ++nparams;
    }
    if (ui->lineEditFilter4CAN->text().length() > 0)
    {
        strncpy(str_param[nparams], ui->lineEditFilter4CAN->text().toStdString().c_str(), MAX_LEN_PARAM);
        str_param[nparams][MAX_LEN_PARAM] = '\0';
        params[nparams] = str_param[nparams];
        ++nparams;
    }
    if (nparams)
    {
        QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdSet, TcpProtocol::eTargetCAN, 1, TcpProtocol::eParamFilter, nparams, params));
        tcpMaster->sendToClient(message);
        ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
    }
}


void MainWindow::on_pushButtonWriteCAN_clicked()
{
    int nparams = 0;
    char str_param[1+8][MAX_LEN_PARAM+1];
    char *params[1+8];

    if (ui->lineEditTxIdCAN->text().length() > 0)
    {
        strncpy(str_param[nparams], ui->lineEditTxIdCAN->text().toStdString().c_str(), MAX_LEN_PARAM);
        str_param[nparams][MAX_LEN_PARAM] = '\0';
        params[nparams] = str_param[nparams];
        ++nparams;
        for (int ind=0; ind<8; ind++)
        {
            QLineEdit *editTxData;
            switch (ind)
            {
            case 0: editTxData = ui->lineEditTxData1CAN; break;
            case 1: editTxData = ui->lineEditTxData2CAN; break;
            case 2: editTxData = ui->lineEditTxData3CAN; break;
            case 3: editTxData = ui->lineEditTxData4CAN; break;
            case 4: editTxData = ui->lineEditTxData5CAN; break;
            case 5: editTxData = ui->lineEditTxData6CAN; break;
            case 6: editTxData = ui->lineEditTxData7CAN; break;
            case 7: editTxData = ui->lineEditTxData8CAN; break;
            }
            if (editTxData->text().length() > 0)
            {
                strncpy(str_param[nparams], editTxData->text().toStdString().c_str(), MAX_LEN_PARAM);
                str_param[nparams][MAX_LEN_PARAM] = '\0';
                params[nparams] = str_param[nparams];
                ++nparams;
            }
        }
        if (nparams >= 2)   // Id CAN + at least 1 byte of data
        {
            QByteArray message(TcpProtocolMaster().toCommand(TcpProtocol::eCmdSend, TcpProtocol::eTargetCAN, 1, TcpProtocol::eParamData, nparams, params));
            tcpMaster->sendToClient(message);
            ui->plainTextEditLog->appendPlainText(QTime::currentTime().toString("hh:mm:ss.zzz") + "    Message sent " + QString(message).chopped(1));
        }
    }
}

