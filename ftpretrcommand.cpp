#include "ftpretrcommand.h"
#include <QFile>
#include <QTcpSocket>

FtpRetrCommand::FtpRetrCommand(QObject *parent, const QString &fileName, qint64 seekTo) :
    FtpCommand(parent)
{
    this->fileName = fileName;
    this->seekTo = seekTo;
    file = 0;
}

FtpRetrCommand::~FtpRetrCommand()
{
    if (started) {
        if (file && file->isOpen() && file->atEnd()) {
            emit reply(226);
        } else {
            emit reply(550);
        }
    }
}

void FtpRetrCommand::startImplementation()
{
    file = new QFile(fileName, this);
    if (!file->open(QIODevice::ReadOnly)) {
        deleteLater();
        return;
    }
    emit reply(150);
    if (seekTo) {
        file->seek(seekTo);
    }
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(refillSocketBuffer(qint64)));
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
