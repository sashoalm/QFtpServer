#ifndef ASYNCHRONOUSRETRIEVECOMMAND_H
#define ASYNCHRONOUSRETRIEVECOMMAND_H

#include "asynchronouscommand.h"

class QFile;

class AsynchronousRetrieveCommand : public AsynchronousCommand
{
    Q_OBJECT
public:
    explicit AsynchronousRetrieveCommand(QObject *parent, const QString &fileName, qint64 seekTo = 0);
    ~AsynchronousRetrieveCommand();

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

#endif // ASYNCHRONOUSRETRIEVECOMMAND_H
