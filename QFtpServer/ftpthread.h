// Author: Kang Lin (kl222@126.com)

#ifndef CBACKGROUNDTHREAD_H_KL_2021_4_4
#define CBACKGROUNDTHREAD_H_KL_2021_4_4

#include <QThread>

class CFtpThread : public QThread
{
    Q_OBJECT
    
public:
    explicit CFtpThread(QString szPath,
                               int nPort,
                               QString szUser = QString(),
                               QString szPassword = QString(),
                               bool bReadOnly = false,
                               bool bOnlyOneIpAllowed = false,
                               QObject *parent = nullptr);
signals:
    void sigMessage(const QString &msg);
    void sigNewPeerIp(const QString &ip);

protected:
    virtual void run() override;
    
private:
    QString m_szPath;
    int m_nPort;
    QString m_szUser;
    QString m_szPassword;
    bool m_bReadOnly;
    bool m_bOnlyOneIpAllowed;
};

#endif // CBACKGROUNDTHREAD_H_KL_2021_4_4
