#include "ftpserver.h"
#include "ftpcontrolconnection.h"
#include "sslserver.h"

#include <QtCore/QDebug>

FtpServer::FtpServer(QObject *parent, const QString &rootPath, int port, const QString &userName, const QString &password) :
    QObject(parent)
{
    server = new SslServer(this);
    server->listen(QHostAddress::Any, port);
    connect(server, SIGNAL(newConnection()), this, SLOT(startNewControlConnection()));
    this->userName = userName;
    this->password = password;
    this->rootPath = rootPath;
}

bool FtpServer::isListening()
{
    return server->isListening();
}

void FtpServer::startNewControlConnection()
{
    qDebug() << "FtpServer::acceptConnection";
    new FtpControlConnection(this, (QSslSocket *) server->nextPendingConnection(), rootPath, userName, password);
}
