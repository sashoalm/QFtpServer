#ifndef ASYNCHRONOUSLISTCOMMAND_H
#define ASYNCHRONOUSLISTCOMMAND_H

#include <QtCore/QObject>

class QTcpSocket;

class AsynchronousListCommand : public QObject
{
    Q_OBJECT
public:
    explicit AsynchronousListCommand(QObject *parent, const QString &listDirectory, bool nameListOnly = false);
    ~AsynchronousListCommand();

signals:
    void reply(int code, const QString &details = QString());

public slots:
    void start(QTcpSocket *socket);

private:
    QTcpSocket* socket;
    QString listDirectory;
    bool nameListOnly;
};

#endif // ASYNCHRONOUSLISTCOMMAND_H
