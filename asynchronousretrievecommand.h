#ifndef ASYNCHRONOUSRETRIEVECOMMAND_H
#define ASYNCHRONOUSRETRIEVECOMMAND_H

#include <QtCore/QObject>

class QFile;
class QTcpSocket;

class AsynchronousRetrieveCommand : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousRetrieveCommand(QObject *parent, const QString &fileName, qint64 seekTo = 0);
    ~AsynchronousRetrieveCommand();

signals:
    void reply(int code, const QString &details = QString());

public slots:
    void start(QTcpSocket *socket);

private slots:
    void refillSocketBuffer(qint64 bytes=0);

private:
    QTcpSocket* socket;
    QString fileName;
    QFile *file;
    qint64 seekTo;
    bool success;
};

#endif // ASYNCHRONOUSRETRIEVECOMMAND_H
