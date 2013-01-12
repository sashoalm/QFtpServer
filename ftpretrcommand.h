#ifndef FTPRETRCOMMAND_H
#define FTPRETRCOMMAND_H

#include "ftpcommand.h"

class QFile;

// the ftp retrieve command
// used to download files from the ftp server

class FtpRetrCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpRetrCommand(QObject *parent, const QString &fileName, qint64 seekTo = 0);
    ~FtpRetrCommand();

private slots:
    void refillSocketBuffer(qint64 bytes=0);

private:
    void startImplementation(QTcpSocket *socket);

    QTcpSocket* socket;
    QString fileName;
    QFile *file;
    qint64 seekTo;
    bool success;
};

#endif // FTPRETRCOMMAND_H
