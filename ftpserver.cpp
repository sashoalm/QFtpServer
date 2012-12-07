#include "ftpserver.h"
#include "ftpcontrolconnection.h"
#include <QtNetwork/QTcpServer>

FtpServer::FtpServer(QObject *parent) :
    QObject(parent)
{
    QTcpServer *server = new QTcpServer;
    server->listen(QHostAddress::Any, 7777);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void FtpServer::acceptConnection()
{
    QTcpServer *server = (QTcpServer *) sender();
    new FtpControlConnection(this, server->nextPendingConnection());
}
