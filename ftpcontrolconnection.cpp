#include "ftpcontrolconnection.h"
#include "asynchronouslistcommand.h"
#include "asynchronousretrievecommand.h"
#include "asynchronousstorecommand.h"
#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtCore/QDebug>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>

FtpControlConnection::FtpControlConnection(QObject *parent, QTcpSocket *socket, const QString &rootPath, const QString &userName, const QString &password) :
    QObject(parent)
{
    this->socket = socket;
    this->userName = userName;
    this->password = password;
    this->rootPath = rootPath;
    isLoggedIn = false;
    socket->setParent(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    currentDirectory = "/";
    dataConnectionServer = new QTcpServer(this);
    connect(dataConnectionServer, SIGNAL(newConnection()), this, SLOT(acceptNewDataConnection()));
    dataConnectionSocket = 0;
    reply(220);
}

FtpControlConnection::~FtpControlConnection()
{
}

void FtpControlConnection::acceptNewData()
{
    buffer += QString::fromUtf8(socket->readAll()).replace("\r\n", "\n").replace('\r', '\n');

    // get the list of complete commands
    bool hasCompleteLine = buffer.endsWith("\n");
    QStringList list = buffer.split("\n", QString::SkipEmptyParts);
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

void FtpControlConnection::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void FtpControlConnection::acceptNewDataConnection()
{
    qDebug() << "Incoming data connection," << (asynchronousCommand ? "starting transfer" : "now waiting for command");
    if (asynchronousCommand)
        asynchronousCommand->start(dataConnectionServer->nextPendingConnection());
    else
        dataConnectionSocket = dataConnectionServer->nextPendingConnection();
    dataConnectionServer->close();
}

void FtpControlConnection::splitCommand(const QString &entireCommand, QString &command, QString &commandParameters)
{
    // split parameters and command
    int pos = entireCommand.indexOf(' ');
    if (-1 != pos) {
        command = entireCommand.left(pos).trimmed().toUpper();
        commandParameters = entireCommand.mid(pos+1).trimmed();
    } else {
        command = entireCommand.trimmed().toUpper();
    }
}

QString FtpControlConnection::toAbsolutePath(const QString &fileName) const
{
    QFileInfo fi(fileName);
    if (!fi.isAbsolute()) {
        fi = QFileInfo(currentDirectory + '/' + fileName);
    }
    QString localPath = QDir::cleanPath(fi.absoluteFilePath());
    if (localPath.startsWith("/../") || localPath == "/..")
        return QString();
    localPath = QDir::cleanPath(rootPath + '/' + localPath);
    qDebug() << "FtpControlConnection::toLocalPath" << fileName << "->" << localPath;
    return localPath;
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
    splitCommand(entireCommand, command, commandParameters);

    if ("USER" == command)
        user(commandParameters);
    else if ("PASS" == command)
        pass(commandParameters);
    else if ("QUIT" == command)
        quit();
    else if (isLoggedIn) {
        if ("PWD" == command)
            reply(227, '"' + currentDirectory + '"');
        else if ("CWD" == command)
            cwd(commandParameters);
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
            reply(350);
        else if ("RNTO" == command)
            rnto(toAbsolutePath(commandParameters));
        else if ("APPE" == command)
            stor(toAbsolutePath(commandParameters), true);
        else if ("REST" == command)
            reply(350);
        else if ("NLST" == command)
            list(toAbsolutePath(""));
        else if ("SIZE" == command)
            size(toAbsolutePath(commandParameters));
        else if ("SYST" == command)
            reply(215);
        else
            reply(502);
    } else {
        reply(530);
    }

    lastProcessedCommand = entireCommand;
}

void FtpControlConnection::startOrScheduleCommand(AsynchronousCommand *asynchronousCommand)
{
    if (!(dataConnectionServer->isListening() || dataConnectionSocket)) {
        delete asynchronousCommand;
        reply(425);
        return;
    }

    this->asynchronousCommand = asynchronousCommand;
    connect(asynchronousCommand, SIGNAL(reply(int,QString)), this, SLOT(reply(int,QString)));
    if (dataConnectionSocket) {
        asynchronousCommand->start(dataConnectionSocket);
        dataConnectionSocket = 0;
    }
}

void FtpControlConnection::pasv()
{
    delete asynchronousCommand;
    if (dataConnectionServer->isListening())
        dataConnectionServer->close();
    dataConnectionServer->listen();
    int port = dataConnectionServer->serverPort();
    reply(227, QString("comment %1,%2,%3").arg(socket->localAddress().toString().replace('.',',')).arg(port/256).arg(port%256));
}

void FtpControlConnection::list(const QString &dir, bool nameListOnly)
{
    startOrScheduleCommand(new AsynchronousListCommand(this, dir, nameListOnly));
}

void FtpControlConnection::retr(const QString &fileName)
{
    startOrScheduleCommand(new AsynchronousRetrieveCommand(this, fileName, seekTo()));
}

void FtpControlConnection::stor(const QString &fileName, bool appendMode)
{
    startOrScheduleCommand(new AsynchronousStoreCommand(this, fileName, appendMode, seekTo()));
}

void FtpControlConnection::cwd(const QString &dir)
{
    QFileInfo fi(toAbsolutePath(dir));
    if (fi.exists() && fi.isDir()) {
        QFileInfo fi(dir);
        if (fi.isAbsolute())
            currentDirectory = QDir::cleanPath(dir);
        else
            currentDirectory = QDir::cleanPath(currentDirectory + '/' + dir);
        reply(250);
    } else {
        reply(550);
    }
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

void FtpControlConnection::rnto(const QString &fileName)
{
    QString command;
    QString commandParameters;
    splitCommand(lastProcessedCommand, command, commandParameters);
    if ("RNFR" == command && QDir().rename(toAbsolutePath(commandParameters), fileName))
        reply(250);
    else
        reply(550);
}

void FtpControlConnection::quit()
{
    reply(221);
    if (asynchronousCommand)
        connect(asynchronousCommand.data(), SIGNAL(destroyed()), this, SLOT(disconnectFromHost()));
    else
        disconnectFromHost();
}

void FtpControlConnection::size(const QString &fileName)
{
    QFileInfo fi(fileName);
    if (fi.isDir())
        reply(550);
    else
        reply(213, QString("%1").arg(fi.size()));
}

void FtpControlConnection::user(const QString &userName)
{
    if (this->userName.isEmpty() || userName == this->userName)
        reply(331);
    else
        reply(530);
}

void FtpControlConnection::pass(const QString &password)
{
    QString command;
    QString commandParameters;
    splitCommand(lastProcessedCommand, command, commandParameters);
    if (this->password.isEmpty() || ("USER" == command && this->userName == commandParameters && this->password == password)) {
        reply(230);
        isLoggedIn = true;
    } else
        reply(530);
}

qint64 FtpControlConnection::seekTo()
{
    qint64 seekTo = 0;
    QString command;
    QString commandParameters;
    splitCommand(lastProcessedCommand, command, commandParameters);
    if ("REST" == command)
        QTextStream(commandParameters.toUtf8()) >> seekTo;
    return seekTo;
}
