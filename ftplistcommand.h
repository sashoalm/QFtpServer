#ifndef FTPLISTCOMMAND_H
#define FTPLISTCOMMAND_H

#include "ftpcommand.h"

class FtpListCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpListCommand(QObject *parent, const QString &listDirectory, bool nameListOnly = false);
    ~FtpListCommand();

private:
    void startImplementation(QTcpSocket *socket);

private:
    QTcpSocket* socket;
    QString listDirectory;
    bool nameListOnly;
};

#endif // FTPLISTCOMMAND_H
