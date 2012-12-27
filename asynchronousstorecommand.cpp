#include "asynchronousstorecommand.h"
#include <QtCore/QFile>
#include <QtNetwork/QTcpSocket>

AsynchronousStoreCommand::AsynchronousStoreCommand(QObject *parent, const QString &fileName, bool appendMode, qint64 seekTo) :
    AsynchronousCommand(parent)
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

void AsynchronousStoreCommand::acceptNextBlock()
{
    file->write(socket->readAll());
}
