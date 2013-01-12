#include "ftpcommand.h"

#include <QtNetwork/QSslSocket>

FtpCommand::FtpCommand(QObject *parent) :
    QObject(parent)
{
}


void FtpCommand::encrypted()
{
    startImplementation((QTcpSocket *) sender());
}


void FtpCommand::start(QTcpSocket *socket, bool encryptDataConnection)
{
    if (encryptDataConnection) {
        QSslSocket *sslSocket = (QSslSocket *) socket;
        connect(sslSocket, SIGNAL(encrypted()), this, SLOT(encrypted()));
        sslSocket->startServerEncryption();
    } else {
        startImplementation(socket);
    }
}
