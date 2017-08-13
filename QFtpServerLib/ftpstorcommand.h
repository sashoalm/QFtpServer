#ifndef FTPSTORCOMMAND_H
#define FTPSTORCOMMAND_H

#include "ftpcommand.h"

class QFile;

// Implements the STOR and APPE commands. Used to upload files to the ftp
// server.

class FtpStorCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpStorCommand(QObject *parent, QFile *file);
    ~FtpStorCommand();

private slots:
    void acceptNextBlock();

private:
    void startImplementation();
    QFile *file;
};

#endif // FTPSTORCOMMAND_H
