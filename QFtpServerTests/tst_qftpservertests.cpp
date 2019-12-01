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
    void testCaseCorrectLogin();
    void testCaseUploadAndDownload();
};

QFtpServerTests::QFtpServerTests()
{
}

// Encapsulates common logic for arranging a test case by creating the
// QFtpServer, a QFtp client, and a root directory for the ftp server (it
// recursively deletes its contents).
class Arrange
{
public:
    // Parameters that can be used to configure the ftp server for testing.
    struct Params
    {
        const char *userName;
        const char *password;
        bool login;
    };

    Arrange(Params *params = 0)
    {
        // Use default params if the user hasn't supplied any.
        Params defaultParams;
        defaultParams.userName = "";
        defaultParams.password = "";
        defaultParams.login = true;
        if (!params) {
            params = &defaultParams;
        }

        // Create the FTP server instance.
        int port = 9421;
        rootPath = "/tmp/ftpservertest/";
        removeRecursively(rootPath);
        QDir().mkpath(rootPath);
        server.reset(new FtpServer(0, rootPath, port, params->userName, params->password));

        // Create the FTP client (and optionally login).
        client.reset(new QFtp());
        client->connectToHost("localhost", port);
        if (params->login) {
            client->login(params->userName, params->password);
        }

        // Create the event loop and connect the client to it
        // so that the event loop stops once the client reports it's ready with an operation.
        loop.reset(new QEventLoop());
        QObject::connect(client.data(), SIGNAL(done(bool)), loop.data(), SLOT(quit()));
    }

    // Removes a folder and all its contents, recursively. We use this because
    // Qt4 lacks QDir::removeRecursively().
    // From https://stackoverflow.com/questions/27758573/deleting-a-folder-and-all-its-contents-with-qt/27758627
    static bool removeRecursively(const QString & dirName)
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

    QScopedPointer<FtpServer> server;
    QScopedPointer<QFtp> client;
    QScopedPointer<QEventLoop> loop;
    QString rootPath;
};

void QFtpServerTests::testCaseMkd()
{
    // Arrange
    Arrange a;

    // Act
    a.client->mkdir("foo");
    a.loop->exec();

    // Assert
    QVERIFY(QDir(a.rootPath + "/foo").exists());
}

void QFtpServerTests::testCaseIncorrectLogin()
{
    // Arrange
    Arrange::Params p = { "user1", "pass1", false };
    Arrange a(&p);

    // Act
    a.client->login("user1", "pass2");
    a.loop->exec();

    // Assert
    QCOMPARE(a.client->error(), QFtp::UnknownError);
}

void QFtpServerTests::testCaseCorrectLogin()
{
    // Arrange
    Arrange::Params p = { "user1", "pass1", false };
    Arrange a(&p);

    // Act
    a.client->login("user1", "pass1");
    a.loop->exec();

    // Assert
    QCOMPARE(a.client->error(), QFtp::NoError);
}

void QFtpServerTests::testCaseUploadAndDownload()
{
    // Arrange
    Arrange a;
    QString fn = "foo.txt";
    QString text = "foo bar";

    // Act
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    a.client->put(text.toAscii(), fn);
    a.loop->exec();
    a.client->get(fn, &buffer);
    a.loop->exec();

    // Assert
    QFile f(a.rootPath + "/" + fn);
    f.open(QIODevice::ReadOnly);
    QVERIFY(f.exists());
    QCOMPARE(QString(f.readAll()), text);
    QCOMPARE(QString(buffer.data()), text);
}

QTEST_MAIN(QFtpServerTests)

#include "tst_qftpservertests.moc"
