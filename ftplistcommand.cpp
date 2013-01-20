#include "ftplistcommand.h"

#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
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

void FtpListCommand::startImplementation()
{
    emit reply(150);

    // start the timer
    index = 0;
    list = new QFileInfoList;
    *list = QDir(listDirectory).entryInfoList();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(listNextBatch()));
    timer->start(0);
}

QString FtpListCommand::fileListingString(const QFileInfo &fi)
{
    // this is how the returned list looks
    // it is like what is returned by 'ls -l'
    // drwxr-xr-x    9 ftp      ftp          4096 Nov 17  2009 pub

    QString line;
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
        QString owner = fi.owner();
        if (owner.isEmpty())
            owner = "unknown";
        QString group = fi.group();
        if (group.isEmpty())
            group = "unknown";
        line += ' ' + owner + ' ' + group;

        // file size
        line += ' ' + QString::number(fi.size());

        // last modified
        QDateTime lm = fi.lastModified();
        line += ' ' + lm.date().toString("MMM d") + ' ' + lm.time().toString("hh:mm");
    }
    line += ' ' + fi.fileName();
    line += "\r\n";
    return line;
}

void FtpListCommand::listNextBatch()
{
    // list next 10 items
    int stop = qMin(index + 10, list->size());
    while (index < stop) {
        QString line = fileListingString(list->at(index));
        socket->write(line.toUtf8());
        index++;
    }

    // if all files have been listed finish
    if (list->size() == stop) {
        delete list;
        timer->stop();
        socket->disconnectFromHost();
    }
}
