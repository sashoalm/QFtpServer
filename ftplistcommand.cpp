#include "ftplistcommand.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtNetwork/QTcpSocket>

FtpListCommand::FtpListCommand(QObject *parent, const QString &fileName, bool nameListOnly) :
    FtpCommand(parent)
{
    this->listDirectory = fileName;
    this->nameListOnly = nameListOnly;
}

FtpListCommand::~FtpListCommand()
{
    emit reply(226);
}

void FtpListCommand::startImplementation(QTcpSocket *socket)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));

    emit reply(150);

    // this is how the returned list looks
    // it is like what is returned by 'ls -l'
    // drwxr-xr-x    9 ftp      ftp          4096 Nov 17  2009 pub

    QString line;
    foreach (QFileInfo fi, QDir(listDirectory).entryInfoList()) {
        if (!nameListOnly) {
            // directory/symlink/file
            if (fi.isSymLink()) line += 'l';
            else if (fi.isDir()) line += 'd';
            else line += '-';

            // permissions
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

            // owner/group
            line += ' ' + fi.owner() + ' ' + fi.group();

            // file size
            line += ' ' + QString::number(fi.size());

            // last modified
            QDateTime lm = fi.lastModified();
            line += ' ' + lm.date().toString("MMM d") + ' ' + lm.time().toString("hh:mm");
        }
        line += ' ' + fi.fileName();
        line += "\r\n";
    }

    socket->write(line.toUtf8());
    socket->disconnectFromHost();
}
