#ifndef ASYNCHRONOUSSTORECOMMAND_H
#define ASYNCHRONOUSSTORECOMMAND_H

#include "asynchronouscommand.h"

class QFile;

class AsynchronousStoreCommand : public AsynchronousCommand
{
    Q_OBJECT
public:
    explicit AsynchronousStoreCommand(QObject *parent, const QString &fileName, bool appendMode = false, qint64 seekTo = 0);
    ~AsynchronousStoreCommand();

private slots:
    void acceptNextBlock();

private:
    void startImplementation(QTcpSocket *socket);

    QTcpSocket* socket;
    QString fileName;
    QFile *file;
    bool appendMode;
    qint64 seekTo;
};

#endif // ASYNCHRONOUSSTORECOMMAND_H
