#include "ftpcontrolconnection.h"
#include "ftppassivedataconnection.h"
#include <QtCore/QStringList>
#include <QtCore/QDir>
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
    else if ("CWD" == command) {
        if (commandParameters.isEmpty()) {
            reply(550);
            return;
        }
        if ('/' != commandParameters[0]) {
            commandParameters = QDir::cleanPath(currentDirectory + '/' + commandParameters);
        }
        if (!QDir().exists(commandParameters)) {
            reply(550);
            return;
        }
        currentDirectory = commandParameters;
        reply(250);
    }
    else if ("TYPE" == command)
        reply(200);
    else if ("PASV" == command)
        pasv();
    else if ("LIST" == command) {
        if (dataConnection)
            dataConnection->list(currentDirectory);
    }
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
