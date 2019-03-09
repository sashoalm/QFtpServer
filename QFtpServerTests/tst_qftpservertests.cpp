#include <QString>
#include <QtTest>
#include <ftpserver.h>
#include <QFtp>

class QFtpServerTests : public QObject
{
    Q_OBJECT

public:
    QFtpServerTests();

private Q_SLOTS:
    void testCaseMkd();
    void testCaseIncorrectLogin();
};

QFtpServerTests::QFtpServerTests()
{
}

// Removes a folder and all its contents, recursively. We use this because
// Qt4 lacks QDir::removeRecursively().
// From https://stackoverflow.com/questions/27758573/deleting-a-folder-and-all-its-contents-with-qt/27758627
bool removeRecursively(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeRecursively(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

void QFtpServerTests::testCaseMkd()
{
    // Arrange
    int port = 9421;
    QString rootPath = "/tmp/ftpservertest/";
    removeRecursively(rootPath);
    QDir().mkpath(rootPath);
    FtpServer server(this, rootPath, port);
    Q_UNUSED(server);

    // Act
    QFtp client;
    client.connectToHost("localhost", port);
    client.login();
    client.mkdir("foo");

    // Wait for the server to finish its work. The
    // ftp client emits a "done()" signal when the
    // last pending command has finished.
    QEventLoop loop;
    connect(&client, SIGNAL(done(bool)), &loop, SLOT(quit()));
    loop.exec();

    // Assert
    QVERIFY(QDir(rootPath + "/foo").exists());
}

void QFtpServerTests::testCaseIncorrectLogin()
{
    // Arrange
    int port = 9421;
    QString rootPath = "/tmp/ftpservertest/";
    removeRecursively(rootPath);
    QDir().mkpath(rootPath);
    FtpServer server(this, rootPath, port, "user1", "pass1");
    Q_UNUSED(server);
    QFtp client;
    QEventLoop loop;
    connect(&client, SIGNAL(done(bool)), &loop, SLOT(quit()));

    // Act
    client.connectToHost("localhost", port);
    client.login("user1", "pass2");
    loop.exec();

    // Assert
    QCOMPARE(client.error(), QFtp::UnknownError);
}

QTEST_MAIN(QFtpServerTests)

#include "tst_qftpservertests.moc"
