#include "ftpcontrolconnection.h"
#include "ftppassivedataconnection.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtNetwork/QTcpSocket>

FtpControlConnection::FtpControlConnection(QObject *parent, QTcpSocket *socket) :
    QObject(parent)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    currentDirectory = "/";
    reply(220);
}

FtpControlConnection::~FtpControlConnection()
{
    reply(221);
}

void FtpControlConnection::acceptNewData()
{
    buffer += QString::fromUtf8(socket->readAll());

    // get the list of complete commands
    bool hasCompleteLine = buffer.endsWith("\r\n");
    QStringList list = buffer.split("\r\n", QString::SkipEmptyParts);
    if (!hasCompleteLine) {
        buffer = list.last();
        list.removeLast();
    } else {
        buffer.clear();
    }

    // process them
    foreach (const QString &line, list) {
        processCommand(line);
    }
}

QString FtpControlConnection::toAbsolutePath(const QString &fileName) const
{
    if (!fileName.isEmpty() && '/' == fileName[0])
        return fileName;
    return QDir::cleanPath(currentDirectory + '/' + fileName);
}

void FtpControlConnection::reply(int code, const QString &details)
{
    qDebug() << "FtpControlConnection::reply" << code << details;

    if (details.isEmpty())
        socket->write(QString("%1 comment\r\n").arg(code).toUtf8());
    else
        socket->write(QString("%1 %2\r\n").arg(code).arg(details).toUtf8());
}

void FtpControlConnection::processCommand(const QString &entireCommand)
{
    qDebug() << "FtpControlConnection::processCommand" << entireCommand;

    QString command;
    QString commandParameters;

    // split parameters and command
    int pos = entireCommand.indexOf(' ');
    if (-1 != pos) {
        command = entireCommand.left(pos).trimmed().toUpper();
        commandParameters = entireCommand.mid(pos+1).trimmed();
    } else {
        command = entireCommand.trimmed().toUpper();
    }

    if ("USER" == command)
        reply(331);
    else if ("PASS" == command)
        reply(230);
    else if ("PWD" == command)
        reply(227, '"' + currentDirectory + '"');
    else if ("CWD" == command)
        cwd(commandParameters);
    else if ("TYPE" == command)
        reply(200);
    else if ("PASV" == command)
        pasv();
    else if ("LIST" == command)
        list(currentDirectory);
    else if ("RETR" == command)
        retr(toAbsolutePath(commandParameters));
    else
        reply(500);
}

void FtpControlConnection::pasv()
{
    delete dataConnection;
    dataConnection = new FtpPassiveDataConnection(this);
    connect(dataConnection.data(), SIGNAL(reply(int,QString)), this, SLOT(reply(int,QString)));
    int port = dataConnection->serverPort();
    reply(227, QString("comment 127,0,0,1,%1,%2").arg(port/256).arg(port%256));
}

void FtpControlConnection::list(const QString &dir)
{
    if (!dataConnection) {
        reply(425);
        return;
    }

    if (!dataConnection->isConnected()) {
        QEventLoop loop;
        connect(dataConnection.data(), SIGNAL(connected()), &loop, SLOT(quit()));
        disconnect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
        loop.exec();
        connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
    }

    reply(150);

    // this is how the returned list looks
    // it is like what is returned by 'ls -l'
    // drwxr-xr-x    9 ftp      ftp          4096 Nov 17  2009 pub

    QString line;
    foreach (QFileInfo fi, QDir(dir).entryInfoList()) {
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
        line += " 1 " + fi.owner() + ' ' + fi.group() + ' ' + QString::number(fi.size()) + ' ';
        QDateTime lm = fi.lastModified();
        line += lm.date().toString("MMM d") + ' ' + lm.time().toString("hh:mm") + ' ';
        line += fi.fileName();
        line += "\r\n";
    }

    dataConnection->socket()->write(line.toUtf8());
    dataConnection->socket()->disconnectFromHost();
    dataConnection = 0;

    reply(226);
}

void FtpControlConnection::retr(const QString &fileName)
{
    if (!dataConnection) {
        reply(425);
        return;
    }
    QFileInfo fi(fileName);
    if (!(fi.exists() && fi.isFile())) {
        reply(550);
        return;
    }
    dataConnection->retr(fileName);
}

void FtpControlConnection::cwd(const QString &_dir)
{
    QString dir = _dir;
    if (dir.isEmpty()) {
        reply(550);
        return;
    }
    if ('/' != dir[0])
        dir = QDir::cleanPath(currentDirectory + '/' + dir);
    if (!QDir().exists(dir)) {
        reply(550);
        return;
    }
    currentDirectory = dir;
    reply(250);
}
