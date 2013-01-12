#include "ftpstorcommand.h"
#include <QtCore/QFile>
#include <QtNetwork/QTcpSocket>

FtpStorCommand::FtpStorCommand(QObject *parent, const QString &fileName, bool appendMode, qint64 seekTo) :
    FtpCommand(parent)
{
    this->fileName = fileName;
    this->appendMode = appendMode;
    file = 0;
    this->seekTo = seekTo;
}

FtpStorCommand::~FtpStorCommand()
{
    emit reply(226);
}

void FtpStorCommand::startImplementation(QTcpSocket *socket)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    file = new QFile(fileName, this);
    if (!file->open(appendMode ? QIODevice::Append : QIODevice::WriteOnly)) {
        emit reply(451);
        deleteLater();
        return;
    }
    emit reply(150);
    if (seekTo)
        file->seek(seekTo);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNextBlock()));
}

void FtpStorCommand::acceptNextBlock()
{
    file->write(socket->readAll());
}
