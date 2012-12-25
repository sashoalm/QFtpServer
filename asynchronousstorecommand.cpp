#include "asynchronousstorecommand.h"
#include "ftppassivedataconnection.h"
#include <QtCore/QFile>
#include <QtNetwork/QTcpSocket>

AsynchronousStoreCommand::AsynchronousStoreCommand(QObject *parent, const QString &fileName, bool appendMode, qint64 seekTo) :
    QObject(parent)
{
    this->fileName = fileName;
    this->appendMode = appendMode;
    file = 0;
    this->seekTo = seekTo;
}

AsynchronousStoreCommand::~AsynchronousStoreCommand()
{
    emit reply(226);
}

void AsynchronousStoreCommand::start(QTcpSocket *socket)
{
    this->socket = socket;
    socket->setParent(this);

    emit reply(150);
    file = new QFile(fileName, this);
    if (appendMode)
        file->open(QIODevice::Append);
    else
        file->open(QIODevice::WriteOnly);
    if (seekTo)
        file->seek(seekTo);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNextBlock()));
}

void AsynchronousStoreCommand::acceptNextBlock()
{
    file->write(socket->readAll());
}
