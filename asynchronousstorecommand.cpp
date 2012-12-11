#include "asynchronousstorecommand.h"
#include "ftppassivedataconnection.h"
#include <QtCore/QFile>
#include <QtNetwork/QTcpSocket>

AsynchronousStoreCommand::AsynchronousStoreCommand(QObject *parent, const QString &fileName) :
    QObject(parent)
{
    this->fileName = fileName;
    file = 0;
}

AsynchronousStoreCommand::~AsynchronousStoreCommand()
{
    emit reply(550);
}

void AsynchronousStoreCommand::start()
{
    emit reply(150);
    file = new QFile(fileName, this);
    file->open(QIODevice::WriteOnly);
    connect(socket(), SIGNAL(readyRead()), this, SLOT(acceptNextBlock()));
}

void AsynchronousStoreCommand::acceptNextBlock()
{
    file->write(socket()->readAll());
}

QTcpSocket *AsynchronousStoreCommand::socket()
{
    FtpPassiveDataConnection *dataConnection = (FtpPassiveDataConnection *) parent();
    return dataConnection->socket();
}
