#include "ftppassivedataconnection.h"

#include <QtCore/QDebug>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

FtpPassiveDataConnection::FtpPassiveDataConnection(QObject *parent) :
    QObject(parent)
{
    _socket = 0;
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptNewConnection()));
    server->listen();
}

int FtpPassiveDataConnection::serverPort()
{
    return server->serverPort();
}

bool FtpPassiveDataConnection::isConnected()
{
    return _socket;
}

void FtpPassiveDataConnection::acceptNewConnection()
{
    qDebug() << "FtpPassiveDataConnection::acceptNewConnection";

    _socket = server->nextPendingConnection();
    _socket->setParent(this);
    connect(_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    delete server;
    server = 0;
    connected();
}
