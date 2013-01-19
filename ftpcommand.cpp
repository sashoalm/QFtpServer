#include "ftpcommand.h"

#include <QtNetwork/QSslSocket>

FtpCommand::FtpCommand(QObject *parent) :
    QObject(parent)
{
}


void FtpCommand::encrypted()
{
    startImplementation();
}


void FtpCommand::start(QTcpSocket *socket, bool encryptDataConnection)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    if (encryptDataConnection) {
        QSslSocket *sslSocket = (QSslSocket *) socket;
        connect(sslSocket, SIGNAL(encrypted()), this, SLOT(encrypted()));
        sslSocket->startServerEncryption();
    } else {
        startImplementation();
    }
}
