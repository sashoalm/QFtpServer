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

public slots:
    void reply(int code, const QString &details = QString());

private slots:
    void acceptNewData();

private:
    QString toAbsolutePath(const QString &fileName) const;
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
