#include "ftpcommand.h"

#include <QSslSocket>

FtpCommand::FtpCommand(QObject *parent) :
    QObject(parent)
{
}

FtpCommand::~FtpCommand()
{
    emit finished(errorText);
}

void FtpCommand::start(QSslSocket *socket)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    startImplementation();
}
