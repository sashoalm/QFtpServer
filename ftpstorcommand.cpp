#include "ftpstorcommand.h"
#include <QFile>
#include <QTcpSocket>

FtpStorCommand::FtpStorCommand(QObject *parent, const QString &fileName, bool appendMode, qint64 seekTo) :
    FtpCommand(parent)
{
    this->fileName = fileName;
    this->appendMode = appendMode;
    file = 0;
    this->seekTo = seekTo;
    success = false;
}

FtpStorCommand::~FtpStorCommand()
{
    if (started) {
        if (success)
            emit reply(226);
        else
            emit reply(451);
    }
}

void FtpStorCommand::startImplementation()
{
    file = new QFile(fileName, this);
    if (!file->open(appendMode ? QIODevice::Append : QIODevice::WriteOnly)) {
        deleteLater();
        return;
    }
    success = true;
    emit reply(150);
    if (seekTo)
        file->seek(seekTo);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNextBlock()));
}

void FtpStorCommand::acceptNextBlock()
{
    file->write(socket->readAll());
}
