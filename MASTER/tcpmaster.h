#ifndef TCPMASTER_H
#define TCPMASTER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QPointer>

class TcpMaster : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpMaster(QObject *parent = nullptr);
    ~TcpMaster() override;

    bool startServer(quint16 port, const QHostAddress &address = QHostAddress::Any);
    void stopServer();

    bool sendToClient(const QByteArray &data);   // invia al client attuale
    QTcpSocket* client() const;                  // restituisce il client attuale
    bool hasClient() const;                      // true se c’è un client connesso

signals:
    void clientConnected(QTcpSocket *client);
    void clientDisconnected(QTcpSocket *client);
    void dataReceived(QTcpSocket *client, const QByteArray &data);
    void clientError(QTcpSocket *client, QAbstractSocket::SocketError socketError);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onReadyRead();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError socketError);

private:
    void registerSocket(QTcpSocket *socket);
    void unregisterSocket();

    QPointer<QTcpSocket> m_client;   // un solo client
};

#endif // TCPMASTER_H
