#include "ftpstorcommand.h"
#include <QFile>
#include <QSslSocket>

FtpStorCommand::FtpStorCommand(QObject *parent, QFile *file) :
    FtpCommand(parent)
{
    this->file = file;
}

FtpStorCommand::~FtpStorCommand()
{
    delete file;
}

void FtpStorCommand::startImplementation()
{
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNextBlock()));
}

void FtpStorCommand::acceptNextBlock()
{
    const QByteArray &bytes = socket->readAll();
    int bytesWritten = file->write(bytes);
    if (bytesWritten != bytes.size()) {
        errorText = "451 Requested action aborted. Could not write data to file.";
        deleteLater();
    }
}
