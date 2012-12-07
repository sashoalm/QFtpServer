#include "ftppassivedataconnection.h"

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

FtpPassiveDataConnection::FtpPassiveDataConnection(QObject *parent) :
    QObject(parent)
{
    socket = 0;
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(acceptNewConnection()));
    server->listen();
    scheduleList = false;
}

int FtpPassiveDataConnection::serverPort()
{
    return server->serverPort();
}

void FtpPassiveDataConnection::list(const QString &dir)
{
    if (!socket) {
        currentDirForListCommand = dir;
        scheduleList = true;
        return;
    }

    reply(150);

    // this is how the returned list looks
    // it is like what is returned by 'ls -l'
    // drwxr-xr-x    9 ftp      ftp          4096 Nov 17  2009 pub

    QString line;
    foreach (QFileInfo fi, QDir(dir).entryInfoList()) {
        if (fi.isSymLink()) line += 'l';
        if (fi.isDir()) line += 'd';
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
        line += " 1 " + fi.owner() + ' ' + fi.group() + ' ' + QString::number(fi.size()) + ' ';
        QDateTime lm = fi.lastModified();
        line += lm.date().toString("MMM d") + ' ' + lm.time().toString("hh:mm") + ' ';
        line += fi.fileName();
        line += '\n';
    }

    socket->write(line.toUtf8());
    socket->disconnectFromHost();

    reply(226);
}

void FtpPassiveDataConnection::acceptNewConnection()
{
    qDebug() << "FtpPassiveDataConnection::acceptNewConnection";

    socket = server->nextPendingConnection();
    socket->setParent(this);
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    delete server;
    server = 0;

    if (scheduleList) {
        scheduleList = false;
        list(currentDirForListCommand);
    }
}
