#ifndef FTPSTORCOMMAND_H
#define FTPSTORCOMMAND_H

#include "ftpcommand.h"

class QFile;

// the ftp stor command
// used to upload files to the ftp server

class FtpStorCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpStorCommand(QObject *parent, const QString &fileName, bool appendMode = false, qint64 seekTo = 0);
    ~FtpStorCommand();

private slots:
    void acceptNextBlock();

private:
    void startImplementation();

    QString fileName;
    QFile *file;
    bool appendMode;
    qint64 seekTo;
};

#endif // FTPSTORCOMMAND_H
