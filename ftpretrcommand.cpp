#include "ftpretrcommand.h"
#include <QtCore/QFile>
#include <QtNetwork/QTcpSocket>

FtpRetrCommand::FtpRetrCommand(QObject *parent, const QString &fileName, qint64 seekTo) :
    FtpCommand(parent)
{
    this->fileName = fileName;
    this->seekTo = seekTo;
    file = 0;
    success = false;
}

FtpRetrCommand::~FtpRetrCommand()
{
    if (success)
        emit reply(226);
    else
        emit reply(550);
}

void FtpRetrCommand::startImplementation()
{
    file = new QFile(fileName, this);
    if (!file->open(QIODevice::ReadOnly)) {
        emit reply(550);
        deleteLater();
        return;
    }
    emit reply(150);
    if (seekTo)
        file->seek(seekTo);
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
    refillSocketBuffer(128*1024);
}

void FtpRetrCommand::refillSocketBuffer(qint64 bytes)
{
    if (!file->atEnd()) {
        socket->write(file->read(bytes));
    } else {
        socket->disconnectFromHost();
        success = true;
    }
}
