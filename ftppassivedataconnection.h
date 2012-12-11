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
    void retr(const QString &fileName);
    void stor(const QString &fileName);

signals:
    void connected();

private slots:
    void acceptNewConnection();

private:
    QTcpSocket *_socket;
    QTcpServer *server;
};

#endif // FTPPASSIVEDATACONNECTION_H
