#include "ftpserver.h"
#include "ftpcontrolconnection.h"
#include <QtNetwork/QTcpServer>

FtpServer::FtpServer(QObject *parent, int port) :
    QObject(parent)
{
    QTcpServer *server = new QTcpServer;
    server->listen(QHostAddress::Any, port);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void FtpServer::acceptConnection()
{
    qDebug() << "FtpServer::acceptConnection";
    QTcpServer *server = (QTcpServer *) sender();
    new FtpControlConnection(this, server->nextPendingConnection());
}
