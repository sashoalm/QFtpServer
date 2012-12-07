#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QObject>

class FtpServer : public QObject
{
    Q_OBJECT
public:
    explicit FtpServer(QObject *parent = 0);
    
signals:
    
private slots:
    void acceptConnection();
};

#endif // FTPSERVER_H
