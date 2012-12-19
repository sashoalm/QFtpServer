#include "asynchronousretrievecommand.h"
#include "ftppassivedataconnection.h"
#include <QtCore/QFile>
#include <QtNetwork/QTcpSocket>

AsynchronousRetrieveCommand::AsynchronousRetrieveCommand(QObject *parent, const QString &fileName, qint64 seekTo) :
    QObject(parent)
{
    this->fileName = fileName;
    this->seekTo = seekTo;
    file = 0;
}

AsynchronousRetrieveCommand::~AsynchronousRetrieveCommand()
{
    if (file && file->isOpen() && file->atEnd())
        emit reply(226);
    else
        emit reply(550);
}

QTcpSocket *AsynchronousRetrieveCommand::socket()
{
    FtpPassiveDataConnection *dataConnection = (FtpPassiveDataConnection *) parent();
    return dataConnection->socket();
}

void AsynchronousRetrieveCommand::start()
{
    emit reply(150);
    file = new QFile(fileName, this);
    if (!file->open(QIODevice::ReadOnly)) {
        emit reply(550);
        deleteLater();
        return;
    }
    if (seekTo)
        file->seek(seekTo);
    connect(socket(), SIGNAL(bytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
    refillSocketBuffer(512*1024);
}

void AsynchronousRetrieveCommand::refillSocketBuffer(qint64 bytes)
{
    socket()->write(file->read(bytes));
    if (file->atEnd()) {
        disconnect(socket(), SIGNAL(bytesWritten(qint64)), this, SLOT(refillSocketBuffer()));
        socket()->disconnectFromHost();
    }
}
