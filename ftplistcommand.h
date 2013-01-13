#ifndef FTPLISTCOMMAND_H
#define FTPLISTCOMMAND_H

#include "ftpcommand.h"

// the ftp list command
// prints out a listing of the given directory
// in the same format as the unix 'ls -l' command

class FtpListCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpListCommand(QObject *parent, const QString &listDirectory, bool nameListOnly = false);
    ~FtpListCommand();

private:
    void startImplementation(QTcpSocket *socket);

private:
    QString listDirectory;
    bool nameListOnly;
};

#endif // FTPLISTCOMMAND_H
