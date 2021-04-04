#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#if defined(Q_OS_ANDROID)
    #include "Notification.h"
#endif

#include "ftpthread.h"

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
    void onMessage(const QString &msg);
    void on_pushButtonShowDebugLog_clicked();
    void on_pushButtonExit_clicked();
    
    // System tray icon
    void slotTrayIconActive(QSystemTrayIcon::ActivationReason e);
    void slotActionExit(bool checked);
#if defined(Q_OS_ANDROID)
    void slotApplicationStateChanged(Qt::ApplicationState state);
    void on_checkBoxDisableLockScreen_stateChanged(int arg1);
#else
    
protected:
    virtual void closeEvent(QCloseEvent *e);    
#endif
    
private:
    Ui::MainWindow *ui;

    // This is the FTP server object.
    CFtpThread *server;

    // Load the stored settings.
    void loadSettings();

    // Save the current settings.
    void saveSettings();

    // Restart the FTP server.
    void startServer();
    
    // System tray icon  
    QSystemTrayIcon m_TrayIcon;
    QMenu m_TrayIconMenu;
    enum _CLOSE_TYPE
    {
        UNKNOW = 0,
        CLOSE = 1,
        HIDE  = 2,
        EXIT  = 4
    };
    int m_nCloseType;
#if defined(Q_OS_ANDROID)
    CNotification m_Notification;
#endif
};

#endif // MAINWINDOW_H
