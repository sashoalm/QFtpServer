#include "ftpserver.h"
#include "ftpcontrolconnection.h"

#include <QtCore/QDebug>
#include <QtNetwork/QTcpServer>

FtpServer::FtpServer(QObject *parent, const QString &rootPath, int port, const QString &userName, const QString &password) :
    QObject(parent)
{
    server = new QTcpServer(this);
    server->listen(QHostAddress::Any, port);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    this->userName = userName;
    this->password = password;
    this->rootPath = rootPath;
}

bool FtpServer::isListening()
{
    return server->isListening();
}

void FtpServer::acceptConnection()
{
    qDebug() << "FtpServer::acceptConnection";
    new FtpControlConnection(this, server->nextPendingConnection(), rootPath, userName, password);
}
