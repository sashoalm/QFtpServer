#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QObject>

class FtpServer : public QObject
{
    Q_OBJECT
public:
    explicit FtpServer(QObject *parent, int port = 21, const QString &userName = QString(), const QString &password = QString());
    
signals:
    
private slots:
    void acceptConnection();

private:
    QString userName;
    QString password;
};

#endif // FTPSERVER_H
