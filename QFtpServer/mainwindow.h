#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class FtpServer;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void on_pushButtonRestartServer_clicked();

    void on_toolButtonBrowse_clicked();

    void onPeerIpChanged(const QString &peerIp);

    void on_pushButtonShowDebugLog_clicked();

    void on_pushButtonExit_clicked();

private:
    Ui::MainWindow *ui;

    // This is the FTP server object.
    FtpServer *server;

    // Load the stored settings.
    void loadSettings();

    // Save the current settings.
    void saveSettings();

    // Restart the FTP server.
    void startServer();

    // Get the LAN IP of the host, e.g. "192.168.1.10".
    QString lanIp();
};

#endif // MAINWINDOW_H
