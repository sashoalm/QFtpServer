#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>

class QSslSocket;

// A simple SSL server. Returns a QSslSocket instead of a QTcpSocket, but
// started in unencrypted mode, so it can be used as a regular QTcpServer, as
// well. The QSslSockets are loaded with a default certificate coming from the
// resource files.

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent);

    // Sets the local certificate and private key for the socket, so
    // startServerEncryption() can be used. We get the local certificate and
    // private key from the application's resources.
    static void setLocalCertificateAndPrivateKey(QSslSocket *socket);

signals:

private:
    void incomingConnection(int socketDescriptor);
};

#endif // SSLSERVER_H
