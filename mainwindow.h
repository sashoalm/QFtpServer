#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

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

private:
    Ui::MainWindow *ui;
    FtpServer *server;
    void loadSettings();
    void saveSettings();
    void startServer();
    void displayLanIpAddress();
};

#endif // MAINWINDOW_H
