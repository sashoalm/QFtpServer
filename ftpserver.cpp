#include "ftpserver.h"
#include "ftpcontrolconnection.h"

#include <QtCore/QDebug>
#include <QtNetwork/QTcpServer>

FtpServer::FtpServer(QObject *parent, int port, const QString &userName, const QString &password) :
    QObject(parent)
{
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, port);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    this->userName = userName;
    this->password = password;
}

bool FtpServer::isListening()
{
    return server->isListening();
}

void FtpServer::acceptConnection()
{
    qDebug() << "FtpServer::acceptConnection";
    new FtpControlConnection(this, server->nextPendingConnection(), userName, password);
}
