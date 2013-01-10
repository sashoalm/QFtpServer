#include "sslserver.h"

#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslKey>

SslServer::SslServer(QObject *parent) :
    QTcpServer(parent)
{
}

void SslServer::incomingConnection(int socketDescriptor)
{
    QSslSocket *socket = new QSslSocket(this);
    if (socket->setSocketDescriptor(socketDescriptor)) {
        socket->setPrivateKey(":/ssl/privkey.pem", QSsl::Rsa, QSsl::Pem, "39129380423984234012312");
        Q_ASSERT(!socket->privateKey().isNull());
        socket->setLocalCertificate(":/ssl/cacert.pem");
        Q_ASSERT(!socket->localCertificate().isNull());
        addPendingConnection(socket);
    } else {
        delete socket;
    }
}
