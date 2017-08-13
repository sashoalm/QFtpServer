#ifndef FTPLISTCOMMAND_H
#define FTPLISTCOMMAND_H

#include "ftpcommand.h"
#include <QFileInfo>
#include <QFileInfoList>

class QTimer;
typedef QList<QFileInfo> QFileInfoList;

// Implements the LIST command. Prints out a listing of the given directory, in
// the same format as the unix 'ls -l' command.

class FtpListCommand : public FtpCommand
{
    Q_OBJECT
public:
    explicit FtpListCommand(QObject *parent, const QFileInfo &listDirectory, bool nameListOnly = false);
    ~FtpListCommand();

private:
    void startImplementation();
    QString fileListingString(const QFileInfo &fi);

private slots:
    void listNextBatch();

private:
    QFileInfo listDirectory;
    bool nameListOnly;
    QTimer *timer;
    QFileInfoList fileInfoList;
    int index;
};

#endif // FTPLISTCOMMAND_H
