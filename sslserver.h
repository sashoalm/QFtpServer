#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>

// a simple SSL server
// returns a QSslSocket instead of a QTcpSocket,
// but started in unencrypted mode, so it can
// be used as a regular QTcpServer, as well
// the QSslSockets are loaded with a default
// certificate coming from the resource files

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent);

signals:

private:
    void incomingConnection(int socketDescriptor);
};

#endif // SSLSERVER_H
