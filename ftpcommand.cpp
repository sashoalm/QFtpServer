#include "ftpcommand.h"

#include <QtNetwork/QSslSocket>

FtpCommand::FtpCommand(QObject *parent) :
    QObject(parent)
{
}

void FtpCommand::start(QTcpSocket *socket)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    startImplementation();
}
