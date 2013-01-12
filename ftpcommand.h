#ifndef FTPCOMMAND_H
#define FTPCOMMAND_H

#include <QObject>

class QTcpSocket;

class FtpCommand : public QObject
{
    Q_OBJECT
public:
    explicit FtpCommand(QObject *parent = 0);

signals:
    void reply(int code, const QString &details = QString());

public:
    void start(QTcpSocket *socket, bool encryptDataConnection);

protected:
    virtual void startImplementation(QTcpSocket *socket) = 0;

private slots:
    void encrypted();
};

#endif // FTPCOMMAND_H
