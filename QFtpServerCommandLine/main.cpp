#include <QCoreApplication>
#include <QDir>
#include <QHostAddress>
#include <QNetworkInterface>
#include <ftpserver.h>
#include <QDebug>
#include <QDateTime>

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

QString lanIp()
{
    // *TODO: Same code in the GUI, move it into FTP Server.
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
            return address.toString();
        }
    }
    return "";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Seed the random numbers.
    qsrand(QTime::currentTime().msecsSinceStartOfDay());

    const QString &userName = getRandomString(3);
    const QString &password = getRandomString(3);
    const QString &rootPath = QDir::currentPath();

    // *TODO: Allow using port 0.
    FtpServer server(&a, rootPath, 2121, userName, password, false, false);
    if (server.isListening()) {
        qDebug().noquote() << QString("Listening at %1:2121").arg(lanIp());
        qDebug().noquote() << QString("User: %1").arg(userName);
        qDebug().noquote() << QString("Password: %1").arg(password);
        return a.exec();
    } else {
        qDebug() << QString("Failed to start");
        return 1;
    }
}
