#include "asynchronouslistcommand.h"
#include "ftppassivedataconnection.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtNetwork/QTcpSocket>

AsynchronousListCommand::AsynchronousListCommand(QObject *parent, const QString &fileName, bool nameListOnly) :
    QObject(parent)
{
    this->listDirectory = fileName;
    this->nameListOnly = nameListOnly;
}

AsynchronousListCommand::~AsynchronousListCommand()
{
    emit reply(226);
}

void AsynchronousListCommand::start()
{
    emit reply(150);

    // this is how the returned list looks
    // it is like what is returned by 'ls -l'
    // drwxr-xr-x    9 ftp      ftp          4096 Nov 17  2009 pub

    QString line;
    foreach (QFileInfo fi, QDir(listDirectory).entryInfoList()) {
        if (!nameListOnly) {
            if (fi.isSymLink()) line += 'l';
            else if (fi.isDir()) line += 'd';
            else line += '-';
            QFile::Permissions p = fi.permissions();
            line += (p & QFile::ReadOwner) ? 'r' : '-';
            line += (p & QFile::WriteOwner) ? 'w' : '-';
            line += (p & QFile::ExeOwner) ? 'x' : '-';
            line += (p & QFile::ReadGroup) ? 'r' : '-';
            line += (p & QFile::WriteGroup) ? 'w' : '-';
            line += (p & QFile::ExeGroup) ? 'x' : '-';
            line += (p & QFile::ReadOther) ? 'r' : '-';
            line += (p & QFile::WriteOther) ? 'w' : '-';
            line += (p & QFile::ExeOther) ? 'x' : '-';
            // number of hard links, we say 1
            line += " " + fi.owner() + ' ' + fi.group() + ' ' + QString::number(fi.size()) + ' ';
            QDateTime lm = fi.lastModified();
            line += lm.date().toString("MMM d") + ' ' + lm.time().toString("hh:mm") + ' ';
        }
        line += fi.fileName();
        line += "\r\n";
    }

    socket()->write(line.toUtf8());
    socket()->disconnectFromHost();
}

QTcpSocket *AsynchronousListCommand::socket()
{
    FtpPassiveDataConnection *dataConnection = (FtpPassiveDataConnection *) parent();
    return dataConnection->socket();
}
