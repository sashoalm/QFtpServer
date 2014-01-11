#ifndef FTPCONTROLCONNECTION_H
#define FTPCONTROLCONNECTION_H

#include <QObject>
#include <QPointer>

class QTcpServer;
class QSslSocket;
class FtpCommand;
class PassiveDataConnection;

// Implements the ftp control connection. Reads the ftp commands from the
// control connection socket, parses each line and maps it to an implemented
// command. All of the ftp commands except the ones that require a data
// connection are implemented here.

class FtpControlConnection : public QObject
{
    Q_OBJECT
public:
    explicit FtpControlConnection(QObject *parent, QSslSocket *socket, const QString &rootPath, const QString &userName = QString(), const QString &password = QString(), bool readOnly = false);
    ~FtpControlConnection();

signals:

public slots:
    void reply(int code, const QString &details = QString());

private slots:
    void acceptNewData();
    void disconnectFromHost();

private:
    void splitCommand(const QString &entireCommand, QString &command, QString &commandParameters);
    QString toLocalPath(const QString &fileName) const;
    void processCommand(const QString &entireCommand);
    void startOrScheduleCommand(FtpCommand *ftpCommand);
    void pasv();
    void list(const QString &dir, bool nameListOnly = false);
    void retr(const QString &fileName);
    void stor(const QString &fileName, bool appendMode = false);
    void cwd(const QString &dir);
    void mkd(const QString &dir);
    void rmd(const QString &dir);
    void dele(const QString &fileName);
    void rnto(const QString &fileName);
    void quit();
    void size(const QString &fileName);
    void pass(const QString &password);
    void auth();
    void prot(const QString &protectionLevel);
    void cdup();
    void feat();
    qint64 seekTo();
    QSslSocket *socket;
    QString currentDirectory;
    QString lastProcessedCommand;
    bool isLoggedIn;
    QString userName;
    QString password;
    QString rootPath;
    bool encryptDataConnection;
    PassiveDataConnection *dataConnection;
    bool readOnly;
};

#endif // FTPCONTROLCONNECTION_H
