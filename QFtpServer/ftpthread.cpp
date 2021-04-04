// Author: Kang Lin (kl222@126.com)

#include "ftpthread.h"
#include "ftpserver.h"

CFtpThread::CFtpThread(QString szPath,
                                     int nPort,
                                     QString szUser,
                                     QString szPassword,
                                     bool bReadOnly,
                                     bool bOnlyOneIpAllowed,
                                     QObject *parent)
    : QThread(parent),
      m_szPath(szPath),
      m_nPort(nPort),
      m_szUser(szUser),
      m_szPassword(szPassword),
      m_bReadOnly(bReadOnly),
      m_bOnlyOneIpAllowed(bOnlyOneIpAllowed)
{
}

void CFtpThread::run()
{
    FtpServer server(nullptr, m_szPath, m_nPort, m_szUser,
                     m_szPassword, m_bReadOnly, m_bOnlyOneIpAllowed);
    bool check = connect(&server, SIGNAL(newPeerIp(const QString&)),
                         this, SIGNAL(sigNewPeerIp(const QString&)));
    Q_ASSERT(check);
    
    if (server.isListening()) {
        emit sigMessage("Listening at " + FtpServer::lanIp());
    } else {
        emit sigMessage("Not listening");
    }
    
    exec();
}
