#include "ftpcontrolconnection.h"
#include "ftplistcommand.h"
#include "ftpretrcommand.h"
#include "ftpstorcommand.h"
#include "sslserver.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QEventLoop>
#include <QtCore/QDebug>
#include <QtCore/QTimer>
#include <QtNetwork/QSslSocket>

FtpControlConnection::FtpControlConnection(QObject *parent, QTcpSocket *socket, const QString &rootPath, const QString &userName, const QString &password) :
    QObject(parent)
{
    this->socket = socket;
    this->userName = userName;
    this->password = password;
    this->rootPath = rootPath;
    isLoggedIn = false;
    encryptDataConnection = false;
    socket->setParent(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    currentDirectory = "/";
    dataConnectionServer = new SslServer(this);
    connect(dataConnectionServer, SIGNAL(newConnection()), this, SLOT(acceptNewDataConnection()));
    dataConnectionSocket = 0;
    isReadyDataConnectionSocket = false;
    reply(220);
}

FtpControlConnection::~FtpControlConnection()
{
}

void FtpControlConnection::acceptNewData()
{
    if (!socket->canReadLine())
        return;
    processCommand(QString::fromUtf8(socket->readLine()).trimmed());
    QTimer::singleShot(0, this, SLOT(acceptNewData()));
}

void FtpControlConnection::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void FtpControlConnection::acceptNewDataConnection()
{
    qDebug() << "Incoming data connection," << (ftpCommand ? "starting transfer" : "now waiting for command");
    QSslSocket *sslSocket = (QSslSocket *) dataConnectionServer->nextPendingConnection();
    sslSocket->setParent(this);
    dataConnectionServer->close();
    dataConnectionSocket = sslSocket;
    if (encryptDataConnection) {
        connect(sslSocket, SIGNAL(encrypted()), this, SLOT(dataConnectionReady()));
        sslSocket->startServerEncryption();
    } else {
        dataConnectionReady();
    }
}

void FtpControlConnection::startFtpCommand()
{
    if (!ftpCommand || !isReadyDataConnectionSocket)
        return;

    ftpCommand->start(dataConnectionSocket);
    dataConnectionSocket = 0;
    isReadyDataConnectionSocket = false;
}

void FtpControlConnection::dataConnectionReady()
{
    isReadyDataConnectionSocket = true;
    startFtpCommand();
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

QString FtpControlConnection::toLocalPath(const QString &fileName) const
{
    QString localPath = fileName;
    localPath.replace('\\', '/');
    if (!localPath.startsWith('/'))
        localPath = currentDirectory + '/' + localPath;

    QStringList components;
    foreach (const QString &component, localPath.split('/', QString::SkipEmptyParts)) {
        if (component == "..") {
            if (components.isEmpty())
                return QString();
            components.pop_back();
        } else if (component != ".") {
            components += component;
        }
    }

    localPath = rootPath;
    foreach (const QString &component, components)
        localPath += '/' + component;
    localPath = QDir::cleanPath(localPath);

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
        reply(331);
    else if ("PASS" == command)
        pass(commandParameters);
    else if ("QUIT" == command)
        quit();
    else if ("AUTH" == command && "TLS" == commandParameters.toUpper())
        auth();
    else if ("FEAT" == command)
        feat();
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
            list(toLocalPath(commandParameters));
        else if ("RETR" == command)
            retr(toLocalPath(commandParameters));
        else if ("STOR" == command)
            stor(toLocalPath(commandParameters));
        else if ("MKD" == command)
            mkd(toLocalPath(commandParameters));
        else if ("RMD" == command)
            rmd(toLocalPath(commandParameters));
        else if ("DELE" == command)
            dele(toLocalPath(commandParameters));
        else if ("RNFR" == command)
            reply(350);
        else if ("RNTO" == command)
            rnto(toLocalPath(commandParameters));
        else if ("APPE" == command)
            stor(toLocalPath(commandParameters), true);
        else if ("REST" == command)
            reply(350);
        else if ("NLST" == command)
            list(toLocalPath(""));
        else if ("SIZE" == command)
            size(toLocalPath(commandParameters));
        else if ("SYST" == command)
            reply(215, "UNIX");
        else if ("PROT" == command)
            prot(commandParameters.toUpper());
        else if ("CDUP" == command)
            cdup();
        else if ("OPTS" == command && "UTF8 ON" == commandParameters.toUpper())
            reply(200);
        else if ("PBSZ" == command && "0" == commandParameters.toUpper())
            reply(200);
        else if ("NOOP" == command)
            reply(200);
        else
            reply(502);
    } else {
        reply(530);
    }

    lastProcessedCommand = entireCommand;
}

void FtpControlConnection::startOrScheduleCommand(FtpCommand *ftpCommand)
{
    if (!dataConnectionServer->isListening() && !dataConnectionSocket) {
        delete ftpCommand;
        reply(425);
        return;
    }

    this->ftpCommand = ftpCommand;
    connect(ftpCommand, SIGNAL(reply(int,QString)), this, SLOT(reply(int,QString)));
    startFtpCommand();
}

void FtpControlConnection::pasv()
{
    delete ftpCommand;
    delete dataConnectionSocket;
    dataConnectionSocket = 0;
    isReadyDataConnectionSocket = false;
    dataConnectionServer->close();
    dataConnectionServer->listen();
    int port = dataConnectionServer->serverPort();
    reply(227, QString("comment %1,%2,%3").arg(socket->localAddress().toString().replace('.',',')).arg(port/256).arg(port%256));
}

void FtpControlConnection::list(const QString &dir, bool nameListOnly)
{
    startOrScheduleCommand(new FtpListCommand(this, dir, nameListOnly));
}

void FtpControlConnection::retr(const QString &fileName)
{
    startOrScheduleCommand(new FtpRetrCommand(this, fileName, seekTo()));
}

void FtpControlConnection::stor(const QString &fileName, bool appendMode)
{
    startOrScheduleCommand(new FtpStorCommand(this, fileName, appendMode, seekTo()));
}

void FtpControlConnection::cwd(const QString &dir)
{
    QFileInfo fi(toLocalPath(dir));
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
    if ("RNFR" == command && QDir().rename(toLocalPath(commandParameters), fileName))
        reply(250);
    else
        reply(550);
}

void FtpControlConnection::quit()
{
    reply(221);
    if (ftpCommand)
        connect(ftpCommand.data(), SIGNAL(destroyed()), this, SLOT(disconnectFromHost()));
    else
        disconnectFromHost();
}

void FtpControlConnection::size(const QString &fileName)
{
    QFileInfo fi(fileName);
    if (!fi.exists() || fi.isDir())
        reply(550);
    else
        reply(213, QString("%1").arg(fi.size()));
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

void FtpControlConnection::auth()
{
    reply(234);
    QSslSocket *sslSocket = (QSslSocket*) socket;
    sslSocket->startServerEncryption();
}

void FtpControlConnection::prot(const QString &protectionLevel)
{
    if ("C" == protectionLevel)
        encryptDataConnection = false;
    else if ("P" == protectionLevel)
        encryptDataConnection = true;
    else {
        reply(502);
        return;
    }
    reply(200);
}

void FtpControlConnection::cdup()
{
    if ("/" == currentDirectory)
        reply(250);
    else
        cwd("..");
}

void FtpControlConnection::feat()
{
    socket->write(
                "211-Features:\r\n"
                " UTF8\r\n"
                "211 End\r\n"
                );
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
