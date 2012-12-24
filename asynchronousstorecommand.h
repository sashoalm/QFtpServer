#ifndef ASYNCHRONOUSSTORECOMMAND_H
#define ASYNCHRONOUSSTORECOMMAND_H

#include <QtCore/QObject>

class QFile;
class QTcpSocket;

class AsynchronousStoreCommand : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousStoreCommand(QObject *parent, const QString &fileName, bool appendMode = false);
    ~AsynchronousStoreCommand();

signals:
    void reply(int code, const QString &details = QString());

public slots:
    void start(QTcpSocket *socket);

private slots:
    void acceptNextBlock();

private:
    QTcpSocket* socket;
    QString fileName;
    QFile *file;
    bool appendMode;
};

#endif // ASYNCHRONOUSSTORECOMMAND_H
