#ifndef ASYNCHRONOUSRETRIEVECOMMAND_H
#define ASYNCHRONOUSRETRIEVECOMMAND_H

#include <QtCore/QObject>

class QFile;
class QTcpSocket;

class AsynchronousRetrieveCommand : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousRetrieveCommand(QObject *parent, const QString &fileName);
    ~AsynchronousRetrieveCommand();

signals:
    void reply(int code, const QString &details = QString());

public slots:
    void start();

private slots:
    void sendNextBlock(qint64 bytes=0);

private:
    QTcpSocket* socket();
    QString fileName;
    QFile *file;
};

#endif // ASYNCHRONOUSRETRIEVECOMMAND_H
