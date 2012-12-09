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
    explicit FtpControlConnection(QObject *parent, QTcpSocket *socket);
    ~FtpControlConnection();
    
signals:
    
private slots:
    void acceptNewData();
    void reply(int code, const QString &details = QString());

private:
    void processCommand(const QString &entireCommand);
    void pasv();
    void list(const QString &dir);
    void retr(const QString &fileName);
    void cwd(const QString &dir);
    QTcpSocket *socket;
    QString buffer;
    QString currentDirectory;
    QPointer<FtpPassiveDataConnection> dataConnection;
};

#endif // FTPCONTROLCONNECTION_H
