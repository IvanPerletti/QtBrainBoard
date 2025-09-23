#include "tcpmaster.h"
#include <QDebug>

TcpMaster::TcpMaster(QObject *parent)
    : QTcpServer(parent)
{
}

TcpMaster::~TcpMaster()
{
    stopServer();
}

bool TcpMaster::startServer(quint16 port, const QHostAddress &address)
{
    if (!this->listen(address, port)) {
        qWarning() << "TcpMaster: Unable to start server:" << this->errorString();
        return false;
    }
    qDebug() << "TcpMaster: Listening on" << this->serverAddress() << ":" << this->serverPort();
    return true;
}

void TcpMaster::stopServer()
{
    if (this->isListening())
        this->close();

    if (m_client) {
        m_client->close();
        // m_client->deleteLater();
        m_client.clear();
    }
}

void TcpMaster::incomingConnection(qintptr socketDescriptor)
{
    if (m_client) {
        // già occupato → rifiuto connessione
        QTcpSocket temp;
        temp.setSocketDescriptor(socketDescriptor);
        temp.disconnectFromHost();
        return;
    }

    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        qWarning() << "TcpMaster: Failed to set socket descriptor:" << socket->errorString();
        socket->deleteLater();
        return;
    }

    registerSocket(socket);
    emit clientConnected(socket);
}

void TcpMaster::registerSocket(QTcpSocket *socket)
{
    m_client = socket;

    connect(socket, &QTcpSocket::readyRead, this, &TcpMaster::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpMaster::onSocketError);
    connect(socket, &QTcpSocket::disconnected, this, &TcpMaster::onDisconnected);
}

void TcpMaster::unregisterSocket()
{
    if (m_client) {
        m_client->deleteLater();
        m_client.clear();
    }
}

void TcpMaster::onReadyRead()
{
    if (!m_client) return;
    QByteArray data = m_client->readAll();
    emit dataReceived(m_client, data);
}

void TcpMaster::onDisconnected()
{
    if (!m_client) return;
    emit clientDisconnected(m_client);
    unregisterSocket();
}

void TcpMaster::onSocketError(QAbstractSocket::SocketError socketError)
{
    if (m_client)
        emit clientError(m_client, socketError);
}

bool TcpMaster::sendToClient(const QByteArray &data)
{
    if (!m_client || m_client->state() != QAbstractSocket::ConnectedState) return false;
    return m_client->write(data) == data.size();
}

QTcpSocket* TcpMaster::client() const
{
    return m_client;
}

bool TcpMaster::hasClient() const
{
    return m_client != nullptr;
}
