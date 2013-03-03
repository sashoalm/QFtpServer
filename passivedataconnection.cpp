#include "passivedataconnection.h"
#include "sslserver.h"
#include "ftpcommand.h"
#include <QtNetwork/QSslSocket>

PassiveDataConnection::PassiveDataConnection(QObject *parent) :
    QObject(parent)
{
    server = new SslServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    socket = 0;
    isSocketReady = false;
    isWaitingForFtpCommand = false;
}

int PassiveDataConnection::listen(bool encrypt)
{
    this->encrypt = encrypt;
    delete socket;
    socket = 0;
    delete command;
    command = 0;
    isSocketReady = false;
    isWaitingForFtpCommand = true;
    server->close();
    server->listen();
    return server->serverPort();
}

bool PassiveDataConnection::setFtpCommand(FtpCommand *command)
{
    if (!isWaitingForFtpCommand)
        return false;
    isWaitingForFtpCommand = false;
    this->command = command;
    command->setParent(this);
    startFtpCommand();
    return true;
}

FtpCommand *PassiveDataConnection::ftpCommand()
{
    if (isSocketReady)
        return command;
    return 0;
}

void PassiveDataConnection::newConnection()
{
    socket = (QSslSocket *) server->nextPendingConnection();
    server->close();
    if (encrypt) {
        connect(socket, SIGNAL(encrypted()), this, SLOT(encrypted()));
        SslServer::setLocalCertificateAndPrivateKey(socket);
        socket->startServerEncryption();
    }
    else
        encrypted();
}

void PassiveDataConnection::encrypted()
{
    isSocketReady = true;
    startFtpCommand();
}

void PassiveDataConnection::startFtpCommand()
{
    if (command && isSocketReady) {
        command->start(socket);
        socket = 0;
    }
}
