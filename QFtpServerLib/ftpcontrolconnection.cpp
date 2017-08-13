#include "ftpcontrolconnection.h"
#include "ftplistcommand.h"
#include "ftpretrcommand.h"
#include "ftpstorcommand.h"
#include "sslserver.h"
#include "dataconnection.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QStringList>
#include <QDir>
#include <QEventLoop>
#include <QDebug>
#include <QTimer>
#include <QSslSocket>

FtpControlConnection::FtpControlConnection(QObject *parent, QSslSocket *socket, const QString &rootPath, const QString &userName, const QString &password, bool readOnly) :
    QObject(parent)
{
    this->socket = socket;
    this->userName = userName;
    this->password = password;
    this->rootPath = rootPath;
    this->readOnly = readOnly;
    isLoggedIn = false;
    encryptDataConnection = false;
    socket->setParent(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(acceptNewData()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    currentDirectory = "/";
    dataConnection = new DataConnection(this);
    reply("220 Welcome to QFtpServer.");
}

FtpControlConnection::~FtpControlConnection()
{
}

void FtpControlConnection::acceptNewData()
{
    if (!socket->canReadLine()) {
        return;
    }

    // Note how we execute only one line, and use QTimer::singleShot, instead
    // of using a for-loop until no more lines are available. This is done
    // so we don't block the event loop for a long time.
    QString response = processCommand(QString::fromUtf8(socket->readLine()).trimmed());
    reply(response);
    QTimer::singleShot(0, this, SLOT(acceptNewData()));

    // Ugly workaround: We have to reply to the AUTH command in plaintext
    // **before** encrypting the channel. We catch the response and do the
    // actual encrypting now.
    if (response == "234 Initializing SSL connection.") {
        SslServer::setLocalCertificateAndPrivateKey(socket);
        socket->startServerEncryption();
    }
}

void FtpControlConnection::disconnectFromHost()
{
    socket->disconnectFromHost();
}

void FtpControlConnection::ftpCommandFinished(const QString &errorText)
{
    if (!errorText.isEmpty()) {
        reply(errorText);
    }
    reply("226 Closing data connection.");
}

bool FtpControlConnection::verifyAuthentication(const QString &command)
{
    if (isLoggedIn) {
        return true;
    }

    const char *commandsRequiringAuth[] = {
        "PWD", "CWD", "TYPE", "PORT", "PASV", "LIST", "RETR", "REST",
        "NLST", "SIZE", "SYST", "PROT", "CDUP", "OPTS", "PBSZ", "NOOP",
        "STOR", "MKD", "RMD", "DELE", "RNFR", "RNTO", "APPE"
    };

    for (size_t ii = 0; ii < sizeof(commandsRequiringAuth)/sizeof(commandsRequiringAuth[0]); ++ii) {
        if (command == commandsRequiringAuth[ii]) {
            return false;
        }
    }

    return true;
}

bool FtpControlConnection::verifyWritePermission(const QString &command)
{
    if (!readOnly) {
        return true;
    }

    const char *commandsRequiringWritePermission[] = {
        "STOR", "MKD", "RMD", "DELE", "RNFR", "RNTO", "APPE"
    };

    for (size_t ii = 0; ii < sizeof(commandsRequiringWritePermission)/sizeof(commandsRequiringWritePermission[0]); ++ii) {
        if (command == commandsRequiringWritePermission[ii]) {
            return false;
        }
    }

    return true;
}

QString FtpControlConnection::stripFlagL(const QString &fileName)
{
    QString a = fileName.toUpper();
    if (a == "-L") {
        return "";
    }
    if (a.startsWith("-L ")) {
        return fileName.mid(3);
    }
    return fileName;
}

void FtpControlConnection::parseCommand(const QString &entireCommand, QString *command, QString *commandParameters)
{
    // Split parameters and command.
    int pos = entireCommand.indexOf(' ');
    if (-1 != pos) {
        *command = entireCommand.left(pos).trimmed().toUpper();
        *commandParameters = entireCommand.mid(pos+1).trimmed();
    } else {
        *command = entireCommand.trimmed().toUpper();
    }
}

QString FtpControlConnection::toLocalPath(const QString &fileName) const
{
    QString localPath = fileName;

    // Some FTP clients send backslashes.
    localPath.replace('\\', '/');

    // If this is a relative path, we prepend the current directory.
    if (!localPath.startsWith('/')) {
        localPath = currentDirectory + '/' + localPath;
    }

    // Evaluate all the ".." and ".", "/path/././to/dir/../.." becomes "/path".
    // Note we do this **before** prepending the root path, in order to avoid
    // "jailbreaking" out of the "chroot".
    QStringList components;
    foreach (const QString &component, localPath.split('/', QString::SkipEmptyParts)) {
        if (component == "..") {
            if (!components.isEmpty()) {
                components.pop_back();
            }
        } else if (component != ".") {
            components += component;
        }
    }

    // Prepend the root path.
    localPath = QDir::cleanPath(rootPath + '/' + components.join("/"));

    qDebug() << "to local path" << fileName << "->" << localPath;
    return localPath;
}

void FtpControlConnection::reply(const QString &text)
{
    qDebug() << "reply" << text;
    socket->write((text + "\r\n").toUtf8());
}

struct AssignmentScopeGuard
{
    AssignmentScopeGuard(QString *last, QString current) : _last(last), _current(current) { }
    ~AssignmentScopeGuard() { *_last = _current; }
    QString *_last, _current;
};

QString FtpControlConnection::processCommand(const QString &entireCommand)
{
    qDebug() << "command" << entireCommand;

    QString command;
    QString commandParameters;
    parseCommand(entireCommand, &command, &commandParameters);

    // lastProcessedCommand needs to be set **after** the return statement.
    AssignmentScopeGuard assignmentScopeGuard(&lastProcessedCommand, entireCommand);
    Q_UNUSED(assignmentScopeGuard);

    if (!verifyAuthentication(command)) {
        return "530 You must log in first.";
    }

    if (!verifyWritePermission(command)) {
        return "550 Can't do that in read-only mode.";
    }

    if ("USER" == command) {
        return "331 User name OK, need password.";
    } else if ("PASS" == command) {
        return pass(commandParameters);
    } else if ("QUIT" == command) {
        return quit();
    } else if ("AUTH" == command && "TLS" == commandParameters.toUpper()) {
        return auth();
    } else if ("FEAT" == command) {
        return feat();
    } else if ("PWD" == command) {
        return QString("257 \"%1\"").arg(currentDirectory);
    } else if ("CWD" == command) {
        return cwd(commandParameters);
    } else if ("TYPE" == command) {
        return "200 Command okay.";
    } else if ("PORT" == command) {
        return port(commandParameters);
    } else if ("PASV" == command) {
        return pasv();
    } else if ("LIST" == command) {
        return list(toLocalPath(stripFlagL(commandParameters)), false);
    } else if ("RETR" == command) {
        return retr(toLocalPath(commandParameters));
    } else if ("REST" == command) {
        return "350 Requested file action pending further information.";
    } else if ("NLST" == command) {
        return list(toLocalPath(stripFlagL(commandParameters)), true);
    } else if ("SIZE" == command) {
        return size(toLocalPath(commandParameters));
    } else if ("SYST" == command) {
        return "215 UNIX";
    } else if ("PROT" == command) {
        return prot(commandParameters.toUpper());
    } else if ("CDUP" == command) {
        return cdup();
    } else if ("OPTS" == command && "UTF8 ON" == commandParameters.toUpper()) {
        return "200 Command okay.";
    } else if ("PBSZ" == command && "0" == commandParameters.toUpper()) {
        return "200 Command okay.";
    } else if ("NOOP" == command) {
        return "200 Command okay.";
    } else if ("STOR" == command) {
        return stor(toLocalPath(commandParameters));
    } else if ("MKD" == command) {
        return mkd(toLocalPath(commandParameters));
    } else if ("RMD" == command) {
        return rmd(toLocalPath(commandParameters));
    } else if ("DELE" == command) {
        return dele(toLocalPath(commandParameters));
    } else if ("RNFR" == command) {
        return "350 Requested file action pending further information.";
    } else if ("RNTO" == command) {
        return rnto(toLocalPath(commandParameters));
    } else if ("APPE" == command) {
        return stor(toLocalPath(commandParameters), true);
    } else {
        return "502 Command not implemented.";
    }
}

QString FtpControlConnection::port(const QString &addressAndPort)
{
    // Example PORT command:
    // PORT h1,h2,h3,h4,p1,p2

    // Get IP and port.
    QRegExp exp("\\s*(\\d+,\\d+,\\d+,\\d+),(\\d+),(\\d+)");
    exp.indexIn(addressAndPort);
    QString hostName = exp.cap(1).replace(',', '.');
    int port = exp.cap(2).toInt() * 256 + exp.cap(3).toInt();
    dataConnection->scheduleConnectToHost(hostName, port, encryptDataConnection);
    return "200 Command okay.";
}

QString FtpControlConnection::pasv()
{
    int port = dataConnection->listen(encryptDataConnection);
    return QString("227 Entering Passive Mode (%1,%2,%3).").arg(socket->localAddress().toString().replace('.',',')).arg(port/256).arg(port%256);
}

QString FtpControlConnection::list(const QString &dir, bool nameListOnly)
{
    if (!dataConnection->isReadyToConnect()) {
        return "425 Can't open data connection.";
    }

    QFileInfo info(dir);
    if (!info.isReadable()) {
        return "425 File or directory is not readable or doesn't exist.";
    }

    FtpListCommand *ftpCommand = new FtpListCommand(this, dir, nameListOnly);
    connect(ftpCommand, SIGNAL(finished(QString)), this, SLOT(ftpCommandFinished(QString)));
    dataConnection->setFtpCommand(ftpCommand);
    return "150 File status okay; about to open data connection.";
}

QString FtpControlConnection::retr(const QString &fileName)
{
    if (!dataConnection->isReadyToConnect()) {
        return "425 Can't open data connection.";
    }

    QFile *file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;
        return "550 Requested action not taken; file unavailable.";
    }

    if (int a = seekTo()) {
        file->seek(a);
    }

    FtpRetrCommand *ftpCommand = new FtpRetrCommand(this, file);
    connect(ftpCommand, SIGNAL(finished(QString)), this, SLOT(ftpCommandFinished(QString)));
    dataConnection->setFtpCommand(ftpCommand);
    return "150 File status okay; about to open data connection.";
}

QString FtpControlConnection::stor(const QString &fileName, bool appendMode)
{
    if (!dataConnection->isReadyToConnect()) {
        return "425 Can't open data connection.";
    }

    QFile *file = new QFile(fileName);
    if (!file->open(appendMode ? QIODevice::Append : QIODevice::WriteOnly)) {
        delete file;
        return "550 Requested action not taken; file unavailable.";
    }

    if (int a = seekTo()) {
        file->seek(a);
    }

    FtpStorCommand *ftpCommand = new FtpStorCommand(this, file);
    connect(ftpCommand, SIGNAL(finished(QString)), this, SLOT(ftpCommandFinished(QString)));
    dataConnection->setFtpCommand(ftpCommand);
    return "150 File status okay; about to open data connection.";
}


QString FtpControlConnection::cwd(const QString &dir)
{
    QFileInfo fi(toLocalPath(dir));
    if (fi.exists() && fi.isDir()) {
        QFileInfo fi(dir);
        if (fi.isAbsolute()) {
            currentDirectory = QDir::cleanPath(dir);
        } else {
            currentDirectory = QDir::cleanPath(currentDirectory + '/' + dir);
        }
        return "250 Requested file action okay, completed.";
    } else {
        return "550 Requested action not taken; file unavailable.";
    }
}

QString FtpControlConnection::mkd(const QString &dir)
{
    if (QDir().mkdir(dir)) {
        return QString("257 \"%1\" created.").arg(dir);
    } else {
        return "550 Requested action not taken; file unavailable.";
    }
}

QString FtpControlConnection::rmd(const QString &dir)
{
    if (QDir().rmdir(dir)) {
        return "250 Requested file action okay, completed.";
    } else {
        return "550 Requested action not taken; file unavailable.";
    }
}

QString FtpControlConnection::dele(const QString &fileName)
{
    if (QDir().remove(fileName)) {
        return "250 Requested file action okay, completed.";
    } else {
        return "550 Requested action not taken; file unavailable.";
    }
}

QString FtpControlConnection::rnto(const QString &fileName)
{
    QString command;
    QString commandParameters;
    parseCommand(lastProcessedCommand, &command, &commandParameters);
    if ("RNFR" == command && QDir().rename(toLocalPath(commandParameters), fileName)) {
        return "250 Requested file action okay, completed.";
    } else {
        return "550 Requested action not taken; file unavailable.";
    }
}

QString FtpControlConnection::quit()
{
    // If we have a running download or upload, we will wait until it's
    // finished before closing the control connection.
    if (dataConnection->ftpCommand()) {
        connect(dataConnection->ftpCommand(), SIGNAL(destroyed()), this, SLOT(disconnectFromHost()));
    } else {
        disconnectFromHost();
    }
    return "221 Quitting...";
}

QString FtpControlConnection::size(const QString &fileName)
{
    QFileInfo fi(fileName);
    if (!fi.exists() || fi.isDir()) {
        return "550 Requested action not taken; file unavailable.";
    } else {
        return QString("213 %1").arg(fi.size());
    }
}

QString FtpControlConnection::pass(const QString &password)
{
    QString command;
    QString commandParameters;
    parseCommand(lastProcessedCommand, &command, &commandParameters);
    if (this->password.isEmpty() || ("USER" == command && this->userName == commandParameters && this->password == password)) {
        isLoggedIn = true;
        return "230 You are logged in.";
    } else {
        return "530 User name or password was incorrect.";
    }
}

QString FtpControlConnection::auth()
{
    // Ugly workaround: We have to reply to the AUTH command in plaintext
    // **before** encrypting the channel. acceptNewData() will do the actual encrypting.
    return "234 Initializing SSL connection.";
}

QString FtpControlConnection::prot(const QString &protectionLevel)
{
    if ("C" == protectionLevel) {
        encryptDataConnection = false;
    } else if ("P" == protectionLevel) {
        encryptDataConnection = true;
    } else {
        return "502 Command not implemented.";
    }
    return "200 Command okay.";
}

QString FtpControlConnection::cdup()
{
    if ("/" == currentDirectory) {
        return "250 Requested file action okay, completed.";
    } else {
        return cwd("..");
    }
}

QString FtpControlConnection::feat()
{
    // We only report that we support UTF8 file names, this is needed because
    // some clients will assume that we use ASCII otherwise, and will not
    // encode the filenames properly.
    return
        "211-Features:\r\n"
        " UTF8\r\n"
        "211 End";
}

qint64 FtpControlConnection::seekTo()
{
    qint64 seekTo = 0;
    QString command;
    QString commandParameters;
    parseCommand(lastProcessedCommand, &command, &commandParameters);
    if ("REST" == command) {
        QTextStream(commandParameters.toUtf8()) >> seekTo;
    }
    return seekTo;
}
