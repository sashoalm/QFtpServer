#include "ftpcontrolconnection.h"
#include "ftppassivedataconnection.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

FtpControlConnection::FtpControlConnection(QObject *parent, QTcpSocket *socket) :
    QObject(parent)
{
    this->socket = socket;
    socket->setParent(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    currentDirectory = QDir::rootPath();
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
    if (!QDir::isAbsolutePath(fileName)) {
        QString newfileName = QDir::cleanPath(QDir(currentDirectory + '/' + fileName).absolutePath());
        qDebug() << "FtpControlConnection::toAbsolutePath" << fileName << "->" << newfileName;
        return newfileName;
    } else {
        return QDir::cleanPath(QDir(fileName).canonicalPath());
    }
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

    // make sure that rnfrStoredFileName is stored only
    // until the RNTO command
    if (!rnfrStoredFileName.isEmpty() && "RNTO" != command)
        rnfrStoredFileName.clear();

    if ("USER" == command)
        reply(331);
    else if ("PASS" == command)
        reply(230);
    else if ("PWD" == command)
        reply(227, '"' + QDir::toNativeSeparators(currentDirectory) + '"');
    else if ("CWD" == command)
        cwd(toAbsolutePath(commandParameters));
    else if ("TYPE" == command)
        reply(200);
    else if ("PASV" == command)
        pasv();
    else if ("LIST" == command)
        list(toAbsolutePath(commandParameters));
    else if ("RETR" == command)
        retr(toAbsolutePath(commandParameters));
    else if ("STOR" == command)
        stor(toAbsolutePath(commandParameters));
    else if ("MKD" == command)
        mkd(toAbsolutePath(commandParameters));
    else if ("RMD" == command)
        rmd(toAbsolutePath(commandParameters));
    else if ("DELE" == command)
        dele(toAbsolutePath(commandParameters));
    else if ("RNFR" == command)
        rnfr(toAbsolutePath(commandParameters));
    else if ("RNTO" == command)
        rnto(toAbsolutePath(commandParameters));
    else
        reply(500);
}

void FtpControlConnection::pasv()
{
    delete dataConnection;
    dataConnection = new FtpPassiveDataConnection(this);
    int port = dataConnection->serverPort();
    reply(227, QString("comment %1,%2,%3").arg(socket->localAddress().toString().replace('.',',')).arg(port/256).arg(port%256));
}

void FtpControlConnection::list(const QString &dir)
{
    if (!dataConnection) {
        reply(425);
        return;
    }

    dataConnection->list(dir);
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

void FtpControlConnection::stor(const QString &fileName)
{
    if (!dataConnection) {
        reply(425);
        return;
    }
    dataConnection->stor(fileName);
}

void FtpControlConnection::cwd(const QString &_dir)
{
    QString dir = _dir;
    if (!QDir::isAbsolutePath(dir))
        dir = currentDirectory + '/' + dir;
    dir = QDir::cleanPath(QDir(dir).canonicalPath());
    if (!QDir().exists(dir)) {
        reply(550);
        return;
    }
    currentDirectory = dir;
    reply(250);
}

void FtpControlConnection::mkd(const QString &dir)
{
    if (QDir().mkdir(dir))
        reply(257);
    else
        reply(550);
}

void FtpControlConnection::rmd(const QString &dir)
{
    if (QDir().rmdir(dir))
        reply(250);
    else
        reply(550);
}

void FtpControlConnection::dele(const QString &fileName)
{
    if (QDir().remove(fileName))
        reply(250);
    else
        reply(550);
}

void FtpControlConnection::rnfr(const QString &fileName)
{
    rnfrStoredFileName = fileName;
    reply(350);
}

void FtpControlConnection::rnto(const QString &fileName)
{
    if (QDir().rename(rnfrStoredFileName, fileName))
        reply(250);
    else
        reply(550);
    rnfrStoredFileName.clear();
}
