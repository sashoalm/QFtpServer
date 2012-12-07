#ifndef FTPPASSIVEDATACONNECTION_H
#define FTPPASSIVEDATACONNECTION_H

#include <QObject>

class QTcpServer;
class QTcpSocket;

class FtpPassiveDataConnection : public QObject
{
    Q_OBJECT
public:
    explicit FtpPassiveDataConnection(QObject *parent = 0);
    int serverPort();
    void list(const QString &dir);

signals:
    void reply(int code, const QString &details = QString());

private slots:
    void acceptNewConnection();

private:
    QTcpSocket *socket;
    QTcpServer *server;
    bool scheduleList;
    QString currentDirForListCommand;
};

#endif // FTPPASSIVEDATACONNECTION_H
