#ifndef FTPCONTROLCONNECTION_H
#define FTPCONTROLCONNECTION_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

class QTcpSocket;
class FtpPassiveDataConnection;

class FtpControlConnection : public QObject
{
    Q_OBJECT
public:
    explicit FtpControlConnection(QObject *parent, QTcpSocket *socket, const QString &userName = QString(), const QString &password = QString());
    ~FtpControlConnection();
    
signals:

public slots:
    void reply(int code, const QString &details = QString());

private slots:
    void acceptNewData();

private:
    void splitCommand(const QString &entireCommand, QString &command, QString &commandParameters);
    QString toAbsolutePath(const QString &fileName) const;
    void processCommand(const QString &entireCommand);
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
    void user(const QString &userName);
    void pass(const QString &password);
    QTcpSocket *socket;
    QString buffer;
    QString currentDirectory;
    QString lastProcessedCommand;
    QPointer<FtpPassiveDataConnection> dataConnection;
    bool isLoggedIn;
    QString userName;
    QString password;
};

#endif // FTPCONTROLCONNECTION_H
