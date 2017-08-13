#include "ftpretrcommand.h"
#include <QFile>
#include <QSslSocket>

FtpRetrCommand::FtpRetrCommand(QObject *parent, QFile *file) :
    FtpCommand(parent)
{
    this->file = file;
}

FtpRetrCommand::~FtpRetrCommand()
{
    delete file;
}

void FtpRetrCommand::startImplementation()
{
    // For encryted SSL sockets, we need to use the encryptedBytesWritten()
    // signal, see the QSslSocket documentation to for reasons why.
    if (socket->isEncrypted()) {
        connect(socket, SIGNAL(encryptedBytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
    } else {
        connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
    }

    refillSocketBuffer(128*1024);
}

void FtpRetrCommand::refillSocketBuffer(qint64 bytes)
{
    if (!file->atEnd()) {
        socket->write(file->read(bytes));
    } else {
        socket->disconnectFromHost();
    }
}
