#include "ftpserver.h"
#include "ftpcontrolconnection.h"

#include <QtCore/QDebug>
#include <QtNetwork/QTcpServer>

FtpServer::FtpServer(QObject *parent, int port, const QString &userName, const QString &password) :
    QObject(parent)
{
    QTcpServer *server = new QTcpServer;
    server->listen(QHostAddress::Any, port);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    this->userName = userName;
    this->password = password;
}

void FtpServer::acceptConnection()
{
    qDebug() << "FtpServer::acceptConnection";
    QTcpServer *server = (QTcpServer *) sender();
    new FtpControlConnection(this, server->nextPendingConnection(), userName, password);
}
