#include "mainwindow.h"

#include <QApplication>
#if defined(Q_OS_ANDROID)
#include <QtAndroid>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QtAndroid::hideSplashScreen();
#endif
    // Needed for QSettings.
    app.setOrganizationName("CodeThesis");
    app.setApplicationName("QFtpServer");

    // Show the main window.
    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
    mainWindow.showExpanded();

    return app.exec();
}
