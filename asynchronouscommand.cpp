#include "asynchronouscommand.h"

#include <QtNetwork/QSslSocket>

AsynchronousCommand::AsynchronousCommand(QObject *parent) :
    QObject(parent)
{
}


void AsynchronousCommand::encrypted()
{
    startImplementation((QTcpSocket *) sender());
}


void AsynchronousCommand::start(QTcpSocket *socket, bool encryptDataConnection)
{
    if (encryptDataConnection) {
        QSslSocket *sslSocket = (QSslSocket *) socket;
        connect(sslSocket, SIGNAL(encrypted()), this, SLOT(encrypted()));
        sslSocket->startServerEncryption();
    } else {
        startImplementation(socket);
    }
}
