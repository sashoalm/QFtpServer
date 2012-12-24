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
    bool isConnected();
    QTcpSocket* socket() { return _socket; }
    void retr(const QString &fileName, qint64 seekTo = 0);
    void stor(const QString &fileName, bool appendMode = false);
    void list(const QString &fileName, bool nameListOnly = false);

signals:
    void connected(QTcpSocket *socket);

private slots:
    void acceptNewConnection();

private:
    void startOrScheduleCommand(QObject *object);
    QTcpSocket *_socket;
    QTcpServer *server;
};

#endif // FTPPASSIVEDATACONNECTION_H
