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
    success = false;
}

AsynchronousRetrieveCommand::~AsynchronousRetrieveCommand()
{
    if (success)
        emit reply(226);
    else
        emit reply(550);
}

void AsynchronousRetrieveCommand::start(QTcpSocket *socket)
{
    this->socket = socket;
    socket->setParent(this);

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
    refillSocketBuffer(512*1024);
}

void AsynchronousRetrieveCommand::refillSocketBuffer(qint64 bytes)
{
    if (!file->atEnd()) {
        socket->write(file->read(bytes));
    } else {
        socket->disconnectFromHost();
        success = true;
    }
}
