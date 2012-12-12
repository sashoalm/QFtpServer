#ifndef ASYNCHRONOUSLISTCOMMAND_H
#define ASYNCHRONOUSLISTCOMMAND_H

#include <QtCore/QObject>

class QTcpSocket;

class AsynchronousListCommand : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousListCommand(QObject *parent, const QString &listDirectory);
    ~AsynchronousListCommand();

signals:
    void reply(int code, const QString &details = QString());

public slots:
    void start();

private:
    QTcpSocket* socket();
    QString listDirectory;
};

#endif // ASYNCHRONOUSLISTCOMMAND_H
