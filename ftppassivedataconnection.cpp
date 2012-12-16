#include "ftppassivedataconnection.h"
#include "asynchronousretrievecommand.h"
#include "asynchronousstorecommand.h"
#include "asynchronouslistcommand.h"

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

void FtpPassiveDataConnection::retr(const QString &fileName, qint64 seekTo)
{
    startOrScheduleCommand(new AsynchronousRetrieveCommand(this, fileName, seekTo));
}

void FtpPassiveDataConnection::stor(const QString &fileName, bool appendMode)
{
    startOrScheduleCommand(new AsynchronousStoreCommand(this, fileName, appendMode));
}

void FtpPassiveDataConnection::list(const QString &fileName)
{
    startOrScheduleCommand(new AsynchronousListCommand(this, fileName));
}

void FtpPassiveDataConnection::acceptNewConnection()
{
    qDebug() << "FtpPassiveDataConnection::acceptNewConnection";

    _socket = server->nextPendingConnection();
    _socket->setParent(this);
    connect(_socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    delete server;
    server = 0;
    emit connected();
}

void FtpPassiveDataConnection::startOrScheduleCommand(QObject *object)
{
    connect(object, SIGNAL(reply(int,QString)), parent(), SLOT(reply(int,QString)));
    connect(this, SIGNAL(connected()), object, SLOT(start()));
    if (_socket)
        emit connected();
}
