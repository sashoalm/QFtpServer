#include <QCoreApplication>
#include <QDir>
#include <ftpserver.h>
#include <QDebug>
#include <QDateTime>
#if defined(Q_OS_ANDROID)
    #include "AndroidUtils.h"
#endif

QChar getRandomChar()
{
    return QChar('a' + (qrand()%('z'-'a')));
}

QString getRandomString(int n)
{
    QString s;
    for (int ii = 0; ii < n; ++ii) {
        s += getRandomChar();
    }
    return s;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#if defined(Q_OS_ANDROID)
    CAndroidUtils::InitPermissions();
#endif
    // Seed the random numbers.
    qsrand(QTime::currentTime().msec());

    const QString &userName = getRandomString(3);
    const QString &password = getRandomString(3);
    const QString &rootPath = QDir::currentPath();

    // *TODO: Allow using port 0.
    FtpServer server(&a, rootPath, 2121, userName, password, false, false);
    if (server.isListening()) {
        qDebug() << QString("Listening at %1:2121").arg(FtpServer::lanIp()).toStdString().c_str();
        qDebug() << QString("User: %1").arg(userName).toStdString().c_str();
        qDebug() << QString("Password: %1").arg(password).toStdString().c_str();
        return a.exec();
    } else {
        qDebug() << QString("Failed to start").toStdString().c_str();
        return 1;
    }
}
