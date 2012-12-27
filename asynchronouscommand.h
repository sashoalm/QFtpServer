#ifndef ASYNCHRONOUSCOMMAND_H
#define ASYNCHRONOUSCOMMAND_H

#include <QObject>

class QTcpSocket;

class AsynchronousCommand : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousCommand(QObject *parent = 0);

signals:
    void reply(int code, const QString &details = QString());

public:
    virtual void start(QTcpSocket *socket) = 0;
};

#endif // ASYNCHRONOUSCOMMAND_H
